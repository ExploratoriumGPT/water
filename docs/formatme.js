async function connectSerial() {
    // Connect to serial port
    try {
        // Wait for existing serial ports to be populated
        const ports = await navigator.serial.getPorts();
        if (ports.length > 0) {
            // Choose the first available port and open it
            port = ports[0];
            await port.open({ baudRate: 115200 });
            console.log('port ' + port + ' opened');
            
            
            
            const signals = await port.getSignals();
            console.log(signals);
            return true;
        }
    } catch (err) {
        console.log('No pre-authorized ports found, requesting user selection');
    }
}


function setupSerialReader() {
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
    
    reader = port.readable.getReader();
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

    function sendSerial(port, char) {
        if (port) {
            const writer = port.writable.getWriter();
            writer.write(new TextEncoder().encode(char));
            writer.releaseLock();
        } else {
            console.info('FAILED to send  ' + char + ' over serial');
        }
    }
    
    
    
    
    
    