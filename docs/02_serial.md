# Serial Connection

## Asynchronous Process

- Initialize connection
- Wait for list of ports
- Open the port
- Test the Signals
- Fail if connection is not made

### Open the Serial Connection

```javascript

  async function connectSerial() {
    // Connect to serial port
    // Step 1
    try {
        // Wait for existing serial ports to be populated
        const ports = await navigator.serial.getPorts();
        if (ports.length > 0) {
            // Choose the first available port and open it
            port = ports[0];
            await port.open({ baudRate: 115200 });
            console.log('port ' + port + ' opened');
        }
    } catch (err) {
        console.log('No pre-authorized ports found, requesting user selection');
    }
    // Step 2
    try {
    		const inputStream = await setupSerialReader( port )
    } catch (err) {
        console.log('Could not configure the Serial Reader');
    }
    // Step 3
    try {
      	const signals = await port.getSignals();
      	console.log(signals);
    } catch (err) {
        console.log('Could not get signals from Serial Port');
    }
    
}
```

Once the connection is made, set up a looping function to respond to new data.

```javascript

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

  

```