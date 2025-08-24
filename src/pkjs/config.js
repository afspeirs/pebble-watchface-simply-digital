const packageJson = require('../../package.json');

module.exports = [
  {
    "type": "heading",
    "defaultValue": "Simply Digital Settings",
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Colour Selection"
      },
      {
        "type": "color",
        "messageKey": "COLOUR_BACKGROUND",
        "label": "Background",
        "defaultValue": "000000",
        "sunlight": true
      },
      {
        "type": "color",
        "messageKey": "COLOUR_HOUR",
        "label": "Hour",
        "defaultValue": "FFAA00",
        "sunlight": true,
        "capabilities": ["NOT_BW"],
      },
      {
        "type": "color",
        "messageKey": "COLOUR_MINUTE",
        "label": "Minute",
        "defaultValue": "FFFFFF",
        "sunlight": true,
        "capabilities": ["NOT_BW"],
      },
      {
        "type": "color",
        "messageKey": "COLOUR_DATE",
        "label": "Date",
        "defaultValue": "FFFFFF",
        "sunlight": true,
        "capabilities": ["NOT_BW"],
      },
    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Bluetooth"
      },
      {
        "type": "toggle",
        "messageKey": "TOGGLE_BLUETOOTH_ICON",
        "label": "Show Bluetooth icon on disconnection",
        "defaultValue": true,
      },
      {
        "type": "select",
        "messageKey": "SELECT_BLUETOOTH_VIBRATION",
        "defaultValue": "2",
        "label": "Bluetooth disconnection Vibration",
        "options": [
          {
            "label": "None",
            "value": "0"
          },
          {
            "label": "Short",
            "value": "1"
          },
          {
            "label": "Long",
            "value": "2"
          },
          {
            "label": "Double",
            "value": "3"
          }
        ]
      },
      {
        "type": "toggle",
        "messageKey": "TOGGLE_BLUETOOTH_QUIET_TIME",
        "label": "Vibrate when Bluetooth disconnects during Quiet Time",
        "defaultValue": false,
      },
    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Battery"
      },
      {
        "type": "select",
        "messageKey": "SELECT_BATTERY_PERCENT",
        "defaultValue": "0",
        "label": "Show battery icon below %",
        "options": [
          {
            "label": "Never",
            "value": "0"
          },
          {
            "label": "10%",
            "value": "10"
          },
          {
            "label": "20%",
            "value": "20"
          },
          {
            "label": "30%",
            "value": "30"
          },
          {
            "label": "40%",
            "value": "40"
          },
          {
            "label": "50%",
            "value": "50"
          },
          {
            "label": "60%",
            "value": "60"
          },
          {
            "label": "70%",
            "value": "70"
          },
          {
            "label": "80%",
            "value": "80"
          },
          {
            "label": "90%",
            "value": "90"
          },
          {
            "label": "100%",
            "value": "100"
          }
        ]
      },
      {
        "type": "toggle",
        "messageKey": "TOGGLE_POWER_SAVE",
        "label": "Enable \"Power Save\" mode",
        "description": "Turn off the display until a flick/shake/tap of the watch",
        "defaultValue": false,
      },
    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Bottom text"
      },
      {
        "type": "toggle",
        "messageKey": "TOGGLE_SUFFIX",
        "label": "Show date suffix (1st, 2nd, 3rd, 4th)",
        "defaultValue": false,
      },
      {
        "type": "toggle",
        "messageKey": "TOGGLE_CALENDAR_WEEK",
        "label": "Show calendar week (W52)",
        "defaultValue": false,
        "capabilities": ["NOT_ROUND"],
      },
      {
        "type": "checkboxgroup",
        "messageKey": "CHECK_DATE",
        "label": "Select events to be displayed:",
        "defaultValue": [false, false, false, false, false, false],
        "options": ["New Year's Day", "Burns Night", "Halloween", "Christmas Eve", "Christmas Day", "Boxing Day"]
      },
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save"
  },
  {
    "type": "text",
    "defaultValue": `Version ${packageJson.version}`,
  },
];
