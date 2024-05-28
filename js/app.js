/* **
*
* Why is this JavaScript file separate from the HTML file?
* - This file is separate from the HTML file to keep the code organized and modular.
* 
* Why is this file a 'Module'?
* - With modules we can use ES6 module syntax to import and export functions and variables.
*
* What if I don't want to use modules?
* - If you don't want to use modules, you can remove 
*   the 'type="module"' attribute from the script tag in the HTML file.
*/

import { connectSerial } from './serial.js';


console.log('---- JavaScript initializing ----');

const pages = window.externalPages;
if ( !pages ) {
    console.error('---- No externalPages found in window object ----');
}

let currentStep = 0;
let currentPage;
let timeoutId;
let port;
const actionButton = document.getElementById('button');
actionButton.addEventListener('click', handleActionButtonClick);
const sequence = [
    'READY',
    'PRE_FILL',
    'FILLING',
    'FULL',
    'PRE_DRIP',
    'POST_DRIP',
    'WGO',
    'RESET'
];

function begin() {
    console.log('---- Begin ----');
    updatePage( sequence[currentStep] );
}

function incrementPage() {
    currentStep++;
    let nextPageName = sequence[currentStep];
    updatePage( nextPageName );
}

function updatePage( pageName ) {
    console.info('updating page' + pageName);
    updateStatus(pageName);
    const page = getPageByName(pageName);
    if ( page.command ) {
        sendCharOverSerial( page.command );
    }
    updatePageContent(page);
    currentPage = page;
}

function updatePageContent( page ) {
    console.log('---- Updating Page Content ----', page);
    
    // Text Content
    updateTitle(page.title);
    text.innerHTML = page.text;
    button.textContent = page.buttonText;
    button.style.backgroundColor = page.buttonColor;
    
    // Image
    image.src = 'svg/' + page.image;
    
    // Button State
    resetButton.disabled = page.lockout;
    button.disabled = page.lockout;
    document.getElementById('resetButton').style.visibility = page.resetVisible ? 'visible' : 'hidden';
}

function lockUI() {
    button.disabled = true;
    resetButton.disabled = true;
}

function unlockUI() {
    button.disabled = false;
    resetButton.disabled = false;
}

function getPageByName(name) {
    // pageIndex = pages.findIndex(page => page.name == 'READY');
    return pages.find( page => page.name === name );
}

function handleActionButtonClick() {
    resetTimeout();
    incrementPage();
}


// Serial Loop

function handleSerialReaderData(data) {
    
    const oneLetterData = `${/[A-Z]/.exec(data)}`;
    // test if variable is a string
    // console.log(typeof oneLetterData);
    //

    console.log('Received:', typeof oneLetterData);
    if (!oneLetterData || oneLetterData == 'null') {
        return;
    }   
    updateFeedback(`Received:  |${oneLetterData}|`);
    // console.log(`Received:  ${oneLetterData}`);

    console.log('Working with page:', currentPage);
    
    // Assume data is formatted as a single character command:
    switch ( oneLetterData ){
        case "N":
            updateFeedback(`Assuming earth tank is filling`);
            // Lock the UI
            // sumpTank is Filling
            break;
        case "F":
            updateFeedback(`Assuming earth tank is full`);  
            // Earth Tank is Full, get ready to drain the earth
            // Unlock the
            // Do nothing and wait for user input

            // clickable: true
            // command: "F\n"
            // image: "welcome.svg"
            // name: "PRE_FILL"
            // resetVisible: true 

            incrementPage();

            break; 
        
        case "V":
            updateFeedback(`Assuming earth tank is draining`);
            // Earth Tank is Draining
            lockUI();
            break;
        
        case "E":
            updateFeedback(`Assuming earth tank is empty`);
            unlockUI();

            // Earth tank is Empty
            // Ready to begin, waiting for user input
            // Do nothing here
            
            break;
            
        case "X":
            // Earth Tank is Full
            updateFeedback(`Assuming earth tank is full2`);

            // console.log('Received command: X, drip dropped');
            // if (pages[pageIndex].name == 'PRE_DRIP') {
            //     incrementPage();
            // }
            break;
        case "D":
            // Sink is Dripping
            break;
        default:
            // Data is not formatted as expected, ignore.
            // updateFeedback(`Received unexpected data: ${oneLetterData}`);
    }
}



function updateTitle(text) {
    console.log('Updating title to: ', text);
    const title = document.getElementById('title');
    title.textContent = text;
}

function updateStatus(text) {
    console.log('Updating status to: ', text);
    const status = document.getElementById('status');
    status.textContent = text;
}

function updateFeedback(text) {
    console.log('Updating feedback to: ', text);
    const feedback = document.getElementById('feedback');
    feedback.textContent = text;
}


// Reset and Timeout

function resetTimeout() {
    // Clear the existing timeout
    // if (timeoutId) {
    //     clearTimeout(timeoutId);
    // }
    
    // // Set a new timeout
    // timeoutId = setTimeout(() => {
    //     console.log('Timeout computer side');
    //     reset();
    // }, 55000); // 55 seconds
}

async function reset() {
    // pageIndex = 0;
    console.log('reset function called');
    sendCharOverSerial('R\n');
    
    // if (tankState.name == 'full' || tankState.name == 'filling' || tankState.name == 'draining') {
    //     console.log('resetting interactive lockout, tank full');
    //     pageIndex = pages.findIndex(page => page.name == 'RESET');
    //     await sendCharOverSerial('T\n');
    // }
    // else {
    //     console.log('resetting interactive FAST, tank not full');
    //     pageIndex = pages.findIndex(page => page.name == 'READY');
    // }
    
    currentStep = 0;
    updatePage( sequence[currentStep] );
}


// Initialize the app

async function init() {
    console.log('---- Initializing... ----');
    try {
        port = await connectSerial();
        begin();
    } catch (err) {
        console.error('Error Initializing: ', err);
        reset();
        
    }
}

window.addEventListener('load', init);

console.log('---- JavaScript Complete ----');
