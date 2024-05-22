const title = document.getElementById('title');
const text = document.getElementById('text');
const button = document.getElementById('button');
const clickColor = '#1be40c';




let pageIndex = 0;
let port;


let tankStates = [
    { name: 'empty', arduinoCharToEnterState: 'E' },// filliNg command to increment state
    { name: 'filling', arduinoCharToEnterState: 'N' },
    { name: 'full', arduinoCharToEnterState: 'F' },//v for vacating
    { name: 'draining', arduinoCharToEnterState: 'V' }
]
let tankState = tankStates.find(state => state.name === 'full');

let debounceTimeout;
// const button = document.getElementById('button');

// // Add event listener to the button
// button.addEventListener('click', async function () {
//     // Disable the button
//     button.disabled = true;
//     button.style.backgroundColor = 'black';


//     // Call the incrementPage function
//     await incrementPage();

//     // Re-enable the button after 500ms
//     setTimeout(function () {
//         button.disabled = false;
//         button.style.backgroundColor = 'blue';

//     }, 500);
// });
button.addEventListener('click', async () => {
    await incrementPage();
});

document.getElementById('resetButton').addEventListener('click', function () {
    reset();
});

async function handleReceivedData(data) {
    console.log(`Received data: ${data}`); // Add this line
    // if (data.includes("\n")) {
    //     //passthru anything that came over the monitor even if it's not a command
    //     console.log(`arduino: ${data}`);
    // }

    // if (pages[pageIndex].incrementCommand) {
    //     console.log('incrementCommand');
    //     if (data.includes(pages[pageIndex].incrementCommand)) {
    //         console.log('incrementing page');
    //         incrementPage();
    //     }
    //     return;
    // }
    //if data includes any of the tankStates commandFromArduinoToIncrement, use them to chaneg tank stat to state +1
    //make an array of tankStates commandFromArduinoToIncrement
    //if data includes uppercase letter, it's a command from arduino
    if (/[A-Z]/.test(data)) {


        let tankStateCommands = tankStates.map(state => state.arduinoCharToEnterState);
        // console.log('all commands' + tankStateCommands)//all commandsE,N,F,V
        if (tankStateCommands.some(command => data.includes(command))) {



            let newState = tankStates.find(state => data.includes(state.arduinoCharToEnterState));
            
            
            if (newState) {
                tankState = newState;
                console.log('Received command: ' + tankState.arduinoCharToEnterState + ', so tank state is now ' + tankState.name);
                // if (tankState.name == 'filling' || tankState.name == 'empty' || tankState.name == 'draining') {
                if (pages[pageIndex].incrementCommand) {
                    console.log('incrementCommand');
                    if (data.includes(pages[pageIndex].incrementCommand)) {
                        // console.log('incrementing page from' + pages[pageIndex].name + ' to ' + pages[(pageIndex + 1) % pages.length].name);
                        incrementPage();
                    }
                }
                //     incrementPage();
                // }
            } else {
                console.log('Received unknown command: ' + data.trim());
            }
        }
        if (data.includes('X')) {
            console.log('Received command: X, drip dropped');
            if (pages[pageIndex].name == 'PRE_DRIP') {
                incrementPage();
            }
        }

    }
}
async function incrementPage() {
    resetTimeout();
    let page = pages[pageIndex];
    let newPageIndex = (pageIndex + 1) % pages.length;

    console.info('INCREMENT PAGE page from' + pages[pageIndex].name + ' to ' + pages[newPageIndex].name);
    if (page.command) {
        sendCharOverSerial(page.command);
    }
    pageIndex = newPageIndex
    updatePage();

}

function updatePage() {
    console.info('updating page' + pageIndex);
    let page = pages[pageIndex];
    title.innerHTML = page.title;
    text.innerHTML = page.text;
    button.textContent = page.buttonText;
    button.style.backgroundColor = page.buttonColor;
    // button.disabled = !page.clickable;

    image.src = 'svg/' + page.image;
    resetButton.disabled = page.lockout;
    button.disabled = page.lockout;
    document.getElementById('resetButton').style.visibility = page.resetVisible ? 'visible' : 'hidden';

}

function sendCharOverSerial(char) {
    if (port) {
        console.info('sending ' + char + ' over serial to arduino');
        const writer = port.writable.getWriter();
        writer.write(new TextEncoder().encode(char));
        writer.releaseLock();
    }
    else {
        console.info('FAILED to send  ' + char + ' over serial');
    }
}

async function readLoop(controller) {
    while (true) {
        // console.log('reading');
        const { value, done } = await reader.read();
        if (value) {
            // console.log('Received:', new TextDecoder().decode(value));
            handleReceivedData(new TextDecoder().decode(value));
        }
        if (done) {
            reader.releaseLock();
            break;
        }
    }
}

let timeoutId;

function resetTimeout() {
    // Clear the existing timeout
    if (timeoutId) {
        clearTimeout(timeoutId);
    }

    // Set a new timeout
    timeoutId = setTimeout(() => {
        console.log('Timeout computer side');
        reset();

    }, 55000); // 55 seconds
}

async function reset() {
    // pageIndex = 0;
    console.log('reset function called');
    sendCharOverSerial('R\n');

    if (tankState.name == 'full' || tankState.name == 'filling' || tankState.name == 'draining') {
        console.log('resetting interactive lockout, tank full');
        pageIndex = pages.findIndex(page => page.name == 'RESET');
        await sendCharOverSerial('T\n');
    }
    else {
        console.log('resetting interactive FAST, tank not full');
        pageIndex = pages.findIndex(page => page.name == 'READY');
    }
    updatePage();
}
// let port;

async function setupSerial() {
    if ('serial' in navigator) {

        
        updateTitle("Aquiring serial port...");

        try {
            const ports = await navigator.serial.getPorts();
            if (ports.length > 0) {
                updateTitle("Serial port found, opening...");
                port = ports[0];
                await port.open({ baudRate: 115200 });
                console.log('port ' + port + ' opened');
                reader = port.readable.getReader();
                inputStream = new ReadableStream({
                    start(controller) {
                        readLoop(controller);
                    }
                });
                const signals = await port.getSignals();
                console.log(signals);
                return true;
            } else {
                updateTitle("No serial port found, can't continue.");
                document.getElementById('serial-help-text').style.display = 'block';
            }
        } catch (err) {
            updateTitle("No serial port found");
            console.log('No pre-authorized ports found, requesting user selection');
        }
  
        return new Promise((resolve, reject) => {
            serialButton.addEventListener('click', async function () {
                try {
                    port = await navigator.serial.requestPort();
                    await port.open({ baudRate: 115200 }); // `baudRate` was `baudrate` in previous versions.
                    console.log('port ' + port + ' opened');
                    reader = port.readable.getReader();
                    inputStream = new ReadableStream({
                        start(controller) {
                            readLoop(controller);
                        }
                    });
                    const signals = await port.getSignals();
                    console.log(signals);
                    serialButton.remove();
                    resolve(true);
                } catch (err) {
                    console.error('There was an error opening the serial port:', err);
                    reject(false);
                }
            });
        });
    } else {
        console.error('Web serial doesn\'t seem to be enabled in your browser. Check https://developer.mozilla.org/en-US/docs/Web/API/Web_Serial_API#browser_compatibility for more info.')

    }
}