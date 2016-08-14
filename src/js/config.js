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
				"messageKey": "COLOUR_DATE",
				"label": "Date",
				"defaultValue": "FFFFFF",
				"sunlight": true,
				"capabilities": ["NOT_BW"],
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
				"messageKey": "COLOUR_BLUETOOTH",
				"label": "Blutooth Disconnect",
				"defaultValue": "FF0000",
				"sunlight": true,
				"capabilities": ["NOT_BW"],
			}
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
				"type": "toggle",
				"messageKey": "TOGGLE_BATTERY",
				"label": "Toggle battery icon",
				"defaultValue": false,
			},
			{
				"type": "select",
				"messageKey": "SELECT_BATTERY_PERCENT",
				"defaultValue": "20",
				"label": "Low battery percent",
				"options": [
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
				]
			},
// 			{
//  				"type": "select",
//  				"messageKey": "SELECT_BATTERY_COLOUR",
//  				"defaultValue": "white",
//  				"label": "Low battery colour:",
//  				"options": [
//  				{ 
//  						"label": "white",
//  						"value": "white" 
// 					},
// 					{ 
// 						"label": "black",
// 						"value": "black" 
// 					},
// 				]
// 			},
		]
 	},
 	{
 		"type": "section",
 		"items": [
			{
				"type": "heading",
				"defaultValue": "Date"
			},	
			{
				"type": "select",
				"messageKey": "SELECT_DATE_BOTTOM",
				"defaultValue": "0",
				"label": "Date layout",
				"options": [
					{ 
						"label": "Day Month",
						"value": "0" 
					},
					{ 
						"label": "Month Day",
						"value": "1" 
					},
				]
			},
			{
				"type": "toggle",
				"messageKey": "TOGGLE_SUFFIX",
				"label": "Toggle suffix",
				"defaultValue": false,
			},
		]
	},
	{
		"type": "section",
		"items": [
			{
				"type": "heading",
				"defaultValue": "Custom Text"
			},
			{
				"type": "checkboxgroup",
				"messageKey": "CHECK_DATE",
				"label": "Select events to be displayed:",
				"defaultValue": [false, false, false],
				"options": ["New Years Day", "Halloween", "Christmas Day"]
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

