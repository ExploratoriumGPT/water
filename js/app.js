
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


// Serial Setup

// Serial setup https://developer.chrome.com/docs/capabilities/serial
// Open Chrome
// - At the top right, click the 3 dots and then Settings.
// - Click Privacy and security and then Site Settings and then Additional permissions.
// - Choose your device type:
//      - USB devices
//      - Serial ports
//      - HID devices
// To choose a default setting, select an option.

async function connectSerial() {
    if (!navigator.serial) {
        updateFeedback('Serial not supported in this browser');
        return;
    }  
    
    let connectedPort = null;
    addSerialListeners();
    updateFeedback("Aquiring serial port...");
    // Step 1
    try {
        // Wait for existing serial ports to be populated
        const ports = await navigator.serial.getPorts();
        if (ports.length > 0) {
            updateFeedback("Serial ports are avaiable... choosing the first one and opening it.");
            // Choose the first available port and open it
            connectedPort = ports[0];
            await connectedPort.open({ baudRate: 115200 });
            // console.log('port ' + connectedPort + ' opened');
            updateFeedback("Serial port opened successfully.");
            
        } else {
            updateFeedback("Aquiring serial port... No ports found!");
            document.getElementById('serial-help-text').style.display = 'block';
            const serialButton = createSerialButton();
            if (serialButton) activateSerialButton(serialButton);
            
        }
    } catch (err) {
        console.log('No pre-authorized ports found, requesting user selection');
    }
    // Step 2
    try {
        updateFeedback("Configuring Serial Reader...");
        if (connectedPort) {
            const inputStream = await setupSerialReader( connectedPort )
        } else {
            updateFeedback("Error: Serial port not connected!");
        }
        updateFeedback("READY!");
    } catch (err) {
        updateFeedback("Error: Could not configure Serial Reader"); 
    }  
    // Step 3
    try {
        const signals = await connectedPort.getSignals();
        console.log('Testing Signals:', signals);
    } catch (err) {
        console.log('Could not get signals from Serial Port');
    }
    
    return connectedPort;
}

async function setupSerialReader(port) {
    // Set up a reader to read data from the serial port
    
    // Adding this function here because it is only used by the reader:
    async function readLoop(controller) {
        while (true) {
            // console.log('reading');
            const { value, done } = await reader.read();
            if (value) {
                // console.log('Received:', new TextDecoder().decode(value));
                handleSerialReaderData(new TextDecoder().decode(value));
            }
            if (done) {
                reader.releaseLock();
                break;
            }
        }
    }
    
    reader = await port.readable.getReader();
    inputStream = new ReadableStream(
        {
            start(controller) {
                // The following function handles each data chunk
                readLoop(controller);
            }
        }
    );
    
    return inputStream;
}


function sendCharOverSerial(char) {
    if (port) {
        updateStatus( `Sending ${char} to arduino`);
        const writer = port.writable.getWriter();
        writer.write(new TextEncoder().encode(char));
        writer.releaseLock();
    }
    else {
        updateStatus( `No Serial Port, Failed to send ${char} to arduino`);
    }
}

function addSerialListeners() {
    navigator.serial.addEventListener("connect", (e) => {
        console.log('Serial port connected');
        // Connect to `e.target` or add it to a list of available ports.
    });
    navigator.serial.addEventListener("disconnect", (e) => {
        console.log('Serial port disconnected');
        // Remove `e.target` from the list of available ports.
    });
}


// UI setup

function createSerialButton() {
    console.log('---- Creating serial button ----');
    if (document.getElementById('serialButton')) {
        return;
    }
    
    // Create serial button
    const serialButton = document.createElement('button');
    serialButton.textContent = 'Press to connect first available serial port';
    serialButton.id = 'serialButton';
    serialButton.style.display = 'inline-block';
    serialButton.style.position = 'absolute';
    serialButton.style.top = '50%';
    serialButton.style.left = '50%';
    serialButton.style.backgroundColor = '#FF0000';
    serialButton.style.transform = 'translate(-50%, -50%)';
    serialButton.style.fontSize = '1.15rem';
    serialButton.style.padding = '1rem 1rem 2rem 1rem';
    serialButton.style.cursor = 'pointer';
    
    // Attach and Display serial button
    return document.body.appendChild(serialButton);
}

function activateSerialButton(serialButton) {
    async function handleButtonClick() {
        updateFeedback("Opening serial connection dialog...");
        const port = await navigator.serial.requestPort();
        
        if (port) {
            console.log('port opened');
            // It's now safe to remove the button
            serialButton.remove();
        }
    }
    
    serialButton.addEventListener('click', handleButtonClick);
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
