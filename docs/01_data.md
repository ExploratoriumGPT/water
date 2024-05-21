# Raw Data



```javascript
  const pages = [
                {
                  pageId: 0, 
                  buttonColor: "#1be40c", 
                  buttonText: "Get started",
                  clickable: true,  
                  image: "welcome.svg",
                  name: "READY", 
                  resetVisible: false,
                  text: "Imagine the Earth were as small as this blow-up globe.<br>How much water would there be?",
                  title: "How much water?"
                },
                {
                  pageId: 1, 
                  buttonColor: "#1be40c", 
                  buttonText: "Drain the Earth", 
                  clickable: true, 
                  command: "F\n",
                  image: "welcome.svg",
                  name: "PRE_FILL", 
                  resetVisible: true,
                  text: "Look at the model of the Earth. Imagine draining all of its water. Drain the oceans, lakes, ice – even water inside living things like you. How full would the tank be?",
                  title: "Try this:"
                },
                {
                  pageId: 2, 
                  buttonColor: "#9aa2a6", 
                  buttonText: "Draining all Earth's water...", 
                  clickable: false, 
                  command: "",
                  image: "welcome.svg",
                  incrementCommand: "F"
                  lockout: true,
                  name: "FILLING", 
                  resetVisible: false, 
                  text: "Look at the model of the Earth. Imagine draining all of its water. Drain the oceans, lakes, ice – even water inside living things like you. How full would the tank be?",
                  title: "Try this:", 
                },
                {
                  pageId: 3, 
                  buttonColor: "#1be40c", 
                  buttonText: "What about fresh water?", 
                  clickable: true, 
                  command: "F\n",
                  image: "welcome.svg",
                  name: "FULL", 
                  resetVisible: true
                  text: "Look at the model of the Earth. Imagine draining all of its water. Drain the oceans, lakes, ice – even water inside living things like you. How full would the tank be?",
                  title: "Try this:", 
                },
                {
                  pageId: 4, 
                  buttonColor: "#9aa2a6", 
                  buttonText: "Put your hand under sink",
                  clickable: false, 
                  image: "drip.svg",
                  incrementCommand: "X",
                  name: "PRE_DRIP", 
                  resetVisible: true,
                  text: "How much water is accessible to humans in lakes, rivers, and groundwater? Put your hand under the sink. That’s the answer!",
                  title: "Try this:"
                },
                {
                  pageId: 5, 
                  buttonColor: "#1be40c", 
                  buttonText: "What's going on?", 
                  clickable: true, 
                  command: "",
                  image: "drip.svg",
                  incrementCommand: "X", 
                  name: "POST_DRIP", 
                  resetVisible: true,
                  text: "How much water is accessible to humans in lakes, rivers, and groundwater? Put your hand under the sink. That’s the answer!",
                  title: "Try this:"
                },
                {
                  pageId: 6, 
                  name: "WGO", 
                  title: "What's going on?", 
                  text: "Earth looks like it has plenty of water: its surface is 71% ocean. But the oceans are a thin layer compared to the rest of the planet. If Earth were an apple, the oceans would be as thick as its skin.<br><br>The majority of Earth’s water isn’t drinkable or accessible. Ocean water is too salty. Most freshwater is held in polar ice caps, or deep underground. Humans rely on the tiny amount remaining to meet all of their needs.",
                  buttonText: "Reset",
                  command: "R\n",
                  buttonColor: "#1be40c",
                  image: "wgo.svg",
                  clickable: true,
                  resetVisible: false
                },
                {
                  pageId: 7,  
                  buttonColor: "#9aa2a6",
                  buttonText: "Putting Earth's water back...",
                  clickable: false,
                  command: "",
                  image: "welcome.svg",
                  incrementCommand: "E"
                  lockout: true,
                  name: "RESET", 
                  resetVisible: false,
                  text: "Imagine the Earth were as small as this blow-up globe.<br>How much water would there be?",
                  title: "How much water?"
                }
            ];
```