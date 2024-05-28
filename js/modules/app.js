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
let currentPage;
let timeoutId;
let port;


// const button = document.getElementById('button');

// button.addEventListener('click', async () => {
//   await incrementPage();
// });

// document.getElementById('resetButton').addEventListener('click', function () {
//   reset();
// });

const actionButton = document.getElementById('button');
actionButton.addEventListener('click', handleActionButtonClick);

function loadPages() {
    fetch('../data/pages.json')
        .then(response => response.json())
        .then(obj => begin(obj))
        .catch(error => {
            // Handle any errors that occurred during the fetch
            console.error('Fetch error:', error);
        });
}

function begin(obj) {
    pages = obj;
    const page = pages[currentStep];
    console.log('---- Begin :: ', page);
   
    updatePage( page );
}

function handleActionButtonClick() {
    resetTimeout();
    incrementPage();
}

function incrementPage() {
    currentStep++;
    const nextPage = pages[currentStep];
    console.log('---- Increment page :: ', nextPage);
    updatePage( nextPage );
}

function updatePage( page ) {
    updateStatus(page.stage);
    console.log("Updating page: ", page);
    updateFeedback(`This page has a next command of: ${page.nextCommand}`);
  
    const actionButton = document.getElementById('button');
    const image = document.getElementById('image');
    const resetButton = document.getElementById('resetButton');
    
    // Image
    image.src = page.graphicsPath;

    if ( page.commandToSend !== undefined && page.commandToSend !== "") {
        serialSend( port, page.commandToSend );
    }
  
    // Button
    actionButton.textContent = page.actionText;
    actionButton.disabled = page.lockout;
    if (page.resetVisible){
      resetButton.style.visibility = 'visible';
      resetButton.disabled = page.lockout;
    } else {
      resetButton.style.visibility = 'hidden';
      resetButton.disabled = page.lockout;
    }
  }
  
  function updateTitle(text) {
      console.log('Updating title to: ', text);
      // const title = document.getElementById('title');
      // title.textContent = text;
  }
  
  function updateStatus(text) {
      console.log('Updating status to: ', text);
      // const status = document.getElementById('status');
      // status.textContent = text;
  }
  
  export { 
    updatePage
  }
  



// Serial Loop

function handleSerialData(data) {
    
    const oneLetterData = `${/[A-Z]/.exec(data)}`;
    if (!oneLetterData || oneLetterData == 'null') {
        // updateFeedback(`Received:  No useful data ${data}` );
        return;
    }   
    updateFeedback(`Received: ${oneLetterData}`);

    updateFeedback(`The current page is expecting command: ${pages[currentStep].nextCommand}`);

    // Assume data is formatted as a single character command:
    switch ( oneLetterData ){
        case "N":
            updateFeedback(`Assuming earth tank is filling... waiting`);
            // Do nothing here
            break;

        case "F":
            updateFeedback(`Assuming earth tank is full... incementing page`);  
            // Earth Tank is Full, get ready to drain the earth
            incrementPage();
            break; 
        
        case "V":
            updateFeedback(`Assuming earth tank is draining`);
            // Do nothing here
            break;
        
        case "E":
            updateFeedback(`Assuming earth tank is empty`);
            if ( pages[currentStep].nextCommand == 'E' ) {
                incrementPage();
            }
            // Do nothing here
            break;
            
        case "X":
            // Earth Tank is Full
            updateFeedback(`X received, drop drop`, pages[currentStep].stage);
            if (pages[currentStep].stage == 'PRE_DRIP') {
                incrementPage();
            }
            break;

        case "D":
            // Sink is Dripping
            break;

        default:
            // Data is not formatted as expected, ignore.
            updateFeedback(`Received unexpected data: ${oneLetterData}`);
    }
}

function lockUI() {
    button.disabled = true;
    resetButton.disabled = true;
  }
  
  function unlockUI() {
    button.disabled = false;
    resetButton.disabled = false;
  }

  function updateFeedback(message, ...args) {
    console.log(`App Feedback: ${message}`, args);
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

    serialSend(port, 'R\n');
    
    // if (tankState.name == 'full' || tankState.name == 'filling' || tankState.name == 'draining') {
    //     console.log('resetting interactive lockout, tank full');
    //     pageIndex = pages.findIndex(page => page.name == 'RESET');
    //     await serialSend('T\n');
    // }
    // else {
    //     console.log('resetting interactive FAST, tank not full');
    //     pageIndex = pages.findIndex(page => page.name == 'READY');
    // }
    
    currentStep = 0;
    const page = pages[currentStep];
    updatePage( page );
}


// Initialize the app

async function init() {
    try {
        const { openPort, stream } = await connectSerial( handleSerialData );
        port = openPort;
        console.log('Port opened, stream ready.', port);
        loadPages();
        
    } catch (err) {
        console.error('Error Initializing: ', err);
        // reset();
    }
}


// async function init() {
//     // console.log('Page Loaded, looking for ports...');
//     const { openPort, stream } = await connectSerial( handleSerialData );
//     port = openPort;
//     console.log('Page Loaded, setupSerial done');
//     if (port) {
//         console.log('setupResult happened!!!!!!!!!!!!!!!!!!!!!!!!!!!');
//         reset();
//     }
// }

window.addEventListener('load', init);
