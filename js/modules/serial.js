/* ------------------------------------  
* 
*  Functions for Serial Connection
*
*    If no serial ports avaialble, follow these steps:
*              Serial setup https://developer.chrome.com/docs/capabilities/serial
*                Open Chrome
*                  - At the top right, click the 3 dots and then Settings.
*                  - Click Privacy and security and then Site Settings and then Additional permissions.
*                  - Choose your device type:
*                        - USB devices
*                        - Serial ports
*                        - HID devices
*                  To choose a default setting, select an option.
*  
* ------------------------------------ */

async function connectSerial( serialCallback ) {
    if (!navigator.serial) {
        updateFeedback('Serial not supported in this browser');
        return;
    }
    let connectedPort = null;
    let readerStream = null;
    try {
        addSerialListeners();
        updateFeedback("Aquiring serial port...");
        connectedPort = await openSerialPort();
        readerStream = await applyReaderToPort(connectedPort, serialCallback);
        checkSignals(connectedPort);
        return { openPort: connectedPort, stream: readerStream };
    } catch (err) {
        //   console.error('[GC DEBUG] Could not connect to serial port: ', err);
        displaySerialFixButton();
        throw err;
    }
}

async function openSerialPort() {
    // Wait for existing serial ports to be populated
    const ports = await navigator.serial.getPorts();
    let connectedPort = null;
    if (ports.length > 0) {
        updateFeedback("Serial ports are avaiable... choosing the first one to open.", ports);
        connectedPort = ports[0];
        await connectedPort.open({ baudRate: 115200 });
        updateFeedback("Serial port opened successfully.");
        
    } else {  
        throw "No serial ports found";
        
    }
    return connectedPort
}

async function applyReaderToPort(openPort, callback) {
    if ( !openPort || !openPort.readable ) {
        throw "Missing an open port to apply the reader to.";
    }
    updateFeedback("Applying Serial Reader to Port...");
    async function setupSerialReader(oPort, callback) {
        // Set up a reader to read data from the serial port
        // Adding this function here because it is only used by the reader:
        async function readLoop(controller, reader) {
            while (true) {
                const { value, done } = await reader.read();
                if (value) {
                    callback(new TextDecoder().decode(value));
                }
                if (done) {
                    reader.releaseLock();
                    break;
                }
            }
        }
        const reader = await oPort.readable.getReader();
        const stream = new ReadableStream(
            {
                start(controller) {
                    // The following function handles each data chunk
                    readLoop(controller, reader);
                }
            }
        );
        return stream;
    }

        const readerStream = await setupSerialReader(openPort, callback)
        updateFeedback("READY!");
        return readerStream;
    
}

async function checkSignals(oPort) {
    try {
        const signals = await oPort.getSignals();
        updateFeedback('Testing Signals:', signals);
    } catch (err) {
        throw ('Could not get signals from Serial Port', err);
    }
}

function addSerialListeners() {
    navigator.serial.addEventListener("connect", (e) => {
        updateFeedback('Serial port connected');
        // Connect to `e.target` or add it to a list of available ports.
    });
    navigator.serial.addEventListener("disconnect", (e) => {
        updateFeedback('Serial port disconnected');
        // Remove `e.target` from the list of available ports.
    });
}

function serialSend(port, char) {
    updateFeedback(`Sending ${char} over serial to arduino`);
    if (port) {
        const writer = port.writable.getWriter();
        writer.write(new TextEncoder().encode(char));
        writer.releaseLock();
    }
    else {
        throw `FAILED to send ${char} over serial. No serial port available.`
    }
}


/* ------------------------------------------------------------------------

    Display a button to allow the user to connect to a serial port

    Transient activation is required. The user has to interact with 
    the page or a UI element in order for this feature to work.

        Note: Transient activation expires after a timeout (if not 
            renewed by further interaction), and may also be "consumed" 
            by some APIs. See Sticky activation for a user activation 
            that doesn't reset after it has been set initially.

    Requires a Secure Context
    https://developer.mozilla.org/en-US/docs/Web/API/Serial/requestPort

    - must be served over https:// or wss:// URLs
    - the security properties of the network channel used to deliver the resource must not be considered deprecated

    Locally-delivered resources (http://127.0.0.1,http://localhost and http://*.localhost) 
    are considered to have been delivered securely.

    ------------------------------------------------------------------------ */
function displaySerialFixButton() {
    if (window.isSecureContext) {
        updateFeedback('Secure context detected. Displaying serial button...');
    } else {
        updateFeedback('Not a secure context. Serial button will not be displayed.');
    }

    const serialButton = createSerialButton();
    async function handleButtonClick() {
        updateFeedback("Opening serial connection dialog...");
        const port = await navigator.serial.requestPort();
        if (port) {
            updateFeedback('port opened');
            // Redo the Init function to re-establish the serial connection
            // init();
            // Throw an event to let the app know that the serial connection is ready
            const event = new CustomEvent('serialready', { detail: { port: port } });
            document.dispatchEvent(event);

            // Clean up the button
            serialButton.remove();
        }
    }
    serialButton.addEventListener('click', handleButtonClick);
}

function createSerialButton() {
    updateFeedback('Creating serial button...');
    if (document.getElementById('serialButton')) {
        return;
    }
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
    return document.body.appendChild(serialButton);
}

/* ------------------------------------  
* 
*  Handle console logs for serial feedback
*  
* ------------------------------------ */

function updateFeedback(message, ...args) {
    console.log(`Feedback [Serial]: ${message}`, (args.length > 0) ? args : '');
}

export { connectSerial, serialSend };

