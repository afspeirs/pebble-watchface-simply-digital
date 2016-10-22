module.exports = [
	{ 
    	"type": "heading", 
    	"defaultValue": "Simply Digital",
		"size": 1,
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
				"type": "color",
				"messageKey": "COLOUR_BLUETOOTH",
				"label": "Blutooth Disconnection",
				"defaultValue": "FF0000",
				"sunlight": true,
				"capabilities": ["NOT_BW"],
			},
			{
				"type": "select",
				"messageKey": "SELECT_BLUETOOTH_DISCONNECT",
				"defaultValue": "2",
				"label": "Vibration",
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
				"label": "Low battery percent",
				"description": "Selecting \"Always show\" will update the icon based on the charge level of the watch",
				"options": [
					{
						"label": "None",
						"value": "0" 
					},
					{
						"label": "10%",
						"value": "11" 
					},
					{
						"label": "20%",
						"value": "21"
					},
					{ 
						"label": "30%",
						"value": "31"
					},
					{ 
						"label": "Always show",
						"value": "100" 
					}
				]
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
				"label": "Show suffix",
				"defaultValue": false,
			},
			{
				"type": "toggle",
				"messageKey": "TOGGLE_WEEK",
				"label": "Show calendar week",
				"defaultValue": false,
				"capabilities": ["NOT_ROUND"],
// 				"description": "Toggle calendar week on bottom",
			},
// 		]
// 	},
// 	{
// 		"type": "section",
// 		"items": [
// 			{
// 				"type": "heading",
// 				"defaultValue": "Custom Text"
// 			},
			{
				"type": "checkboxgroup",
				"messageKey": "CHECK_DATE",
				"label": "Select events to be displayed:",
				"defaultValue": [false, false, false, false, false],
				"options": ["New Year's Day", "Burns Night", "Halloween", "Christmas Day", "Boxing Day"]
			}
		]
	},
// 	{
// 		"type": "section",
// 		"items": [
// 			{
// 				"type": "heading",
// 				"defaultValue": "Custom Date"
// 			},
// 			{
// 				"type": "input",
// 				"messageKey": "Custon date",
// 				"label": "Test date thing",
// 				"attributes": {
// 					"type": "date"
// 				}
// 			}
// 		]
// 	},
	{
		"type": "submit",
		"defaultValue": "Save"
	}
];

