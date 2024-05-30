/* **
*
* Why is this JavaScript file separate from the HTML file?
* - This file is separate from the HTML file to keep the code 
*   organized and modular.
* 
* Why is this file a 'Module'?
* - With modules we can use ES6 module syntax to import and 
*   export functions and variables.
* - We can also import JSON directly.
*
* Why can't I open this file directly in a browser?
* - The default CORS policy in most browsers prevents loading 
*   modules from the local file system.
*
* How do I view this file in a brower then?
* - Set up a local server using Python or Node.js, 
*   or use a tool like Live Server in VS Code.
*   - For Python, navigate to the directory and run `python -m http.server`.
*   - For Node.js, install 'http-server' globally and 
*     run 'http-server' in the directory.
*
* What if I don't want to use modules?
* - If you don't want to use modules, you can remove 
*   the 'type="module"' attribute from the script tag
*   in the HTML file.
*/

import { connectSerial, serialSend } from './serial.js';

let pages;
let currentStep = 0;
let timeoutId;
let serialPort;
let largeTankState = '';
let startupStatus = true;


const titleText = document.getElementById('overlay-title');
const statusText = document.getElementById('overlay-status');
const feedbackText = document.getElementById('overlay-feedback');
titleText.innerHTML = "JavaScript Starting...";
statusText.innerHTML = "Loading...";



// Dynamic DOM Elements
const imageElement = document.getElementById('image');
const resetButton = document.getElementById('resetButton');
const actionButton = document.getElementById('button');

actionButton.addEventListener('click', handleActionButtonClick);

console.log(window.location.href);

function loadPages() {
    logInfo("Loading Pages...");
    fetch('./data/pages.json')
    .then(response => response.json())
    .then(obj => begin(obj))
    .catch(error => {
        // Handle any errors that occurred during the fetch
        console.error('Fetch error:', error);
    });
}

function begin(obj) {
    logInfo("Begin");
    pages = obj;
    updatePage( currentStep );
    displayStartupStatus(3);
}

function incrementPage() {
    currentStep++;
    logInfo(`---- Increment page #${currentStep} ---- `);
    updatePage( currentStep );
}

function updatePage( pageIndex ) {
    if (pageIndex == 2) {
        displayStartupStatus(4);
    }
    if ( startupStatus ){
        // if we made it here, we are past the startup sequence.
        displayStartupStatus(10);
        startupStatus = false;
    }
    
    const nextPage = pages[pageIndex];
    logStatus(nextPage.stage);
    
    logInfo("Updating page: ", nextPage);
    logInfo(`This page has an increment command of: ${nextPage.pageIncrementCommand}`);
    
    // Image
    imageElement.src = nextPage.graphicsPath;
    
    if ( nextPage.pageLoadedCommand !== undefined && nextPage.pageLoadedCommand !== "") {
        serialSend( serialPort, nextPage.pageLoadedCommand );
    }
    
    // Button
    actionButton.textContent = nextPage.actionText;
    actionButton.disabled = nextPage.lockout;
    if (nextPage.resetVisible){
        resetButton.style.visibility = 'visible';
        resetButton.disabled = nextPage.lockout;
    } else {
        resetButton.style.visibility = 'hidden';
        resetButton.disabled = nextPage.lockout;
    }
}

/*

Events:

Actions that are triggered at unpredictable times.

- User interactions events, like button clicks.
- Serial data events.

*/

function handleActionButtonClick() {
    resetTimeout();
    incrementPage();
}

document.addEventListener('serialready', handleSerialReady);

function handleSerialReady(event) {
    logInfo('Serial Ready Event: ', event);
    displayStartupStatus(2);
    serialPort = event.detail.port;
    loadPages();
}

function handleSerialData(data) {
    logInfo(`Raw data:: ${data}` );
    const oneLetterData = `${/[A-Z]/.exec(data)}`;
    if (!oneLetterData || oneLetterData == 'null') {
        // logInfo(`Received:  No useful data ${data}` );
        return;
    }   
    logInfo(`Received ${oneLetterData}, ${pages[currentStep].pageIncrementCommand} will trigger page increment.`);
    // Assume data is formatted as a single character command:
    switch ( oneLetterData ){
        case "N":
        largeTankState = 'N';
        logInfo(`Assuming earth tank is filling... Do nothing here`);
        break;
        
        case "F":
        largeTankState = 'F';
        logInfo(`FULL, ready to drain. Incrementing page.`);  
        incrementPage();
        break; 
        
        case "V":
        largeTankState = 'V';
        logInfo(`Assuming earth tank is draining... Do nothing here`);
        break;
        
        case "E":
        largeTankState = 'E';
        logInfo(`Assuming earth tank is empty... increment page if needed`);
        if ( pages[currentStep].pageIncrementCommand == 'E' ) {
            incrementPage();
        }
        break;
        
        case "X":
        largeTankState = 'X';
        logInfo(`Actively dripping:: `, pages[currentStep].stage);
        resetTimeout();
        if (pages[currentStep].stage == 'PRE_DRIP') {
            incrementPage();
        }
        break;
        
        case "D":
        largeTankState = 'D';
        logInfo(`Actively dripping [Possibly Unused Stage]:: `, pages[currentStep].stage);
        break;
        
        default:
        logInfo(`Received unexpected data: ${oneLetterData}`);
    }
}

// Reset and Timeout

function resetTimeout() {
    clearTheTimeout();
    timeoutId = setTimeout(() => {
        reset('timeout');
    }, 55000); // 55 seconds
}

function clearTheTimeout() {
    if (timeoutId) {
        clearTimeout(timeoutId);
    }
}

async function reset(reason) {
    logInfo(`reset function called: ${reason}, tank state: ${largeTankState}`);
    if (largeTankState == 'E') {
        logInfo('Tank already empty, ready for next visitor.');
        currentStep = 1;
    } else {
        logInfo('Tank is full, draining...');
        serialSend(serialPort, 'R\n');
        currentStep = 0;
    }
    updatePage( currentStep );
}

/* 

Logging

    Why go through the trouble of creating logging functions?

    - Logging functions provide a consistent way to log messages.
    - They can be turned off or modified easily.
    - They can be used to add more information like timestamps 
    and the section of the code that is initiating the logging.

*/
function logStatus(message, ...args) {
    console.log(`Status [app]: ${message}`, args);
    statusText.textContent = message;
}

function logInfo(message, ...args) {
    console.log(`Info [app]: ${message}`, args);
    feedbackText.textContent = `${feedbackText.textContent}<br>${message}`;
}

function logError(message, ...args) {
    console.error(`Error [app]: ${message}`, ...args);
}


/*

    Start Up Status Display

    - Display a list of steps on the screen that can aid debugging.
    - This only displayed on startup.
    - Let's the admin know at what stage the app is at when and if it fails.

*/

function displayStartupStatus(step) {
    const step1 = document.querySelector('#load-sequence .step-1');
    const step2 = document.querySelector('#load-sequence .step-2');
    const step3 = document.querySelector('#load-sequence .step-3');
    const step4 = document.querySelector('#load-sequence .step-4');
    const step5 = document.querySelector('#load-sequence .step-5');

    switch (step) {
        case 1:
        logInfo('JAvaScript loaded');
        step1.classList.remove("wait");
        step1.classList.add("check");
        break;
        
        case 2:
        logInfo('Connected to Serial Port');
        step2.classList.remove("wait");
        step2.classList.add("check");
        break;
        
        case 3:
        logInfo('Pages Loaded');
        step3.classList.remove("wait");
        step3.classList.add("check");
        break;
        
        case 4:
        logInfo('Tank Reset');
        step4.classList.remove("wait");
        step4.classList.add("check");
        break;

        case 5:
        logInfo('Ready');
        step5.classList.remove("wait");
        step5.classList.add("check");
        break;

        case 10:
        logInfo('Removing Startup List');
        const loadSequence = document.getElementById('load-sequence');
        loadSequence.classList.remove("show");
        loadSequence.classList.add("hide");
        break;
        
        default:
        logInfo('Unknown Step');
    }
}


// Initialize the app

async function init() {
    displayStartupStatus(1);
    try {
        feedbackText.innerHTML = "Initing...";
        const { openPort, stream } = await connectSerial( handleSerialData );
        serialPort = openPort;
        displayStartupStatus(2);
        logInfo('Port opened, stream ready.', serialPort, stream);
        loadPages();
        
    } catch (err) {
        logError('[GC DEBUG] Error Initializing: ', err);
        feedbackText.innerHTML = "Error connecting to serial port.";
    }
}

window.addEventListener('load', init);
