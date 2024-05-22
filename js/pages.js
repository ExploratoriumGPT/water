window.externalPages = [
  {
    "name": "READY",
    "title": "How much water?",
    "text": "Imagine the Earth were as small as this blow-up globe.<br>How much water would there be?",
    "buttonText": "Get started",
    "buttonColor": "#1be40c",
    "image": "welcome.svg",
    "clickable": true,
    "resetVisible": false
  },
  {
    "name": "PRE_FILL",
    "title": "Try this:",
    "text": "Look at the model of the Earth. Imagine draining all of its water. Drain the oceans, lakes, ice – even water inside living things like you. How full would the tank be?",
    "buttonText": "Drain the Earth",
    "command": "F\n",
    "buttonColor": "#1be40c",
    "image": "welcome.svg",
    "clickable": true,
    "resetVisible": true
  },
  {
    "name": "FILLING",
    "title": "Try this:",
    "text": "Look at the model of the Earth. Imagine draining all of its water. Drain the oceans, lakes, ice – even water inside living things like you. How full would the tank be?",
    "buttonText": "Draining all Earth's water...",
    "command": "",
    "buttonColor": "#9aa2a6",
    "image": "welcome.svg",
    "clickable": false,
    "resetVisible": false,
    "lockout": true,
    "incrementCommand": "F"
  },
  {
    "name": "FULL",
    "title": "Try this:",
    "text": "Look at the model of the Earth. Imagine draining all of its water. Drain the oceans, lakes, ice – even water inside living things like you. How full would the tank be?",
    "buttonText": "What about fresh water?",
    "command": "F\n",
    "buttonColor": "#1be40c",
    "image": "welcome.svg",
    "clickable": true,
    "resetVisible": true
  },
  {
    "name": "PRE_DRIP",
    "title": "Try this:",
    "text": "How much water is accessible to humans in lakes, rivers, and groundwater? Put your hand under the sink. That’s the answer!",
    "buttonText": "Put your hand under sink",
    "buttonColor": "#9aa2a6",
    "image": "drip.svg",
    "clickable": false,
    "resetVisible": true,
    "incrementCommand": "X"
  },
  {
    "name": "POST_DRIP",
    "title": "Try this:",
    "text": "How much water is accessible to humans in lakes, rivers, and groundwater? Put your hand under the sink. That’s the answer!",
    "buttonText": "What's going on?",
    "buttonColor": "#1be40c",
    "image": "drip.svg",
    "clickable": true,
    "resetVisible": true,
    "incrementCommand": "X",
    "command": ""
  },
  {
    "name": "WGO",
    "title": "What's going on?",
    "text": "Earth looks like it has plenty of water: its surface is 71% ocean. But the oceans are a thin layer compared to the rest of the planet. If Earth were an apple, the oceans would be as thick as its skin.<br><br>The majority of Earth’s water isn’t drinkable or accessible. Ocean water is too salty. Most freshwater is held in polar ice caps, or deep underground. Humans rely on the tiny amount remaining to meet all of their needs.",
    "buttonText": "Reset",
    "command": "R\n",
    "buttonColor": "#1be40c",
    "image": "wgo.svg",
    "clickable": true,
    "resetVisible": false
  },
  {
    "name": "RESET",
    "title": "How much water?",
    "text": "Imagine the Earth were as small as this blow-up globe.<br>How much water would there be?",
    "buttonText": "Putting Earth's water back...",
    "buttonColor": "#9aa2a6",
    "image": "welcome.svg",
    "clickable": false,
    "resetVisible": false,
    "command": "",
    "lockout": true,
    "incrementCommand": "E"
  }
];