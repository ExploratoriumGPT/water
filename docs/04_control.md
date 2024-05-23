# Operation Functions


```javascript

  function handleSerialReaderData(data) {
        // Assume data is formatted as a single character command:
        switch ( data.trim().toUpperCase() ){
            case "E":
                // sumpTank is Empty
                break;
            case "F":
                // sumpTank is Full
                break;
            case "N":
                // sumpTank is Filling
                break;
            case "V":
                // earthTank is Draining
                break;
            case "X":
                 // earthTank is Full
                console.log('Received command: X, drip dropped');
                if (pages[pageIndex].name == 'PRE_DRIP') {
                    incrementPage();
                }
                break;
            case "D":
                // Sink is Dripping
                break;
            default:
                // Data is not formatted as expected, ignore.    
        }
    }

  ```

# Reset Functions

```javascript
    
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


  ```