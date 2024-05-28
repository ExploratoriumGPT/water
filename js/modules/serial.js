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
      console.error('Could not connect to serial port: ', err);
      displaySerialFixButton();
  }
}

async function openSerialPort() {
  // Wait for existing serial ports to be populated
  const ports = await navigator.serial.getPorts();
  let connectedPort = null;
  if (ports.length <= 0) {
      throw "No serial ports found";

  } else {

    updateFeedback("Ports: ", ports);
      updateFeedback("Serial ports are avaiable... choosing the first one to open.");
      connectedPort = ports[0];
      await connectedPort.open({ baudRate: 115200 });
      updateFeedback("Serial port opened successfully.");

  }
  return connectedPort
}

async function applyReaderToPort(openPort, callback) {
  updateFeedback("Applying Serial Reader to Port...");
  async function setupSerialReader(oPort, callback) {
      // Set up a reader to read data from the serial port
      // Adding this function here because it is only used by the reader:
      async function readLoop(controller, reader) {
          while (true) {
              // console.log('reading');
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

  if (openPort) {
      const readerStream = await setupSerialReader(openPort, callback)
      updateFeedback("READY!");
      return readerStream;
  } else {
      throw "Missing an open port.";
  }
}

async function checkSignals(oPort) {
  try {
      const signals = await oPort.getSignals();
      console.log('Testing Signals:', signals);
  } catch (err) {
        updateFeedback('Could not get signals from Serial Port');
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
    console.info('sending ' + char + ' over serial to arduino');
    if (port) {
        const writer = port.writable.getWriter();
        writer.write(new TextEncoder().encode(char));
        writer.releaseLock();
    }
    else {
        console.info('FAILED to send  ' + char + ' over serial');
    }
}




/* ------------------------------------
* 
*  Display a button to allow the user to connect to a serial port
*  
* ------------------------------------ */

function displaySerialFixButton() {
  const serialButton = createSerialButton();
  async function handleButtonClick() {
      updateFeedback("Opening serial connection dialog...");
      const port = await navigator.serial.requestPort();
      if (port) {
          console.log('port opened');
          // Redo the Init function to re-establish the serial connection
          init();
          // Clean up the button
          serialButton.remove();
      }
  }
  serialButton.addEventListener('click', handleButtonClick);
}
function createSerialButton() {
  console.log('---- Creating serial button ----');
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
  console.log(`Serial Feedback: ${message}`, args);
}


export { connectSerial, serialSend };

