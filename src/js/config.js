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
				"label": "Background Colour",
				"defaultValue": "000000",
				"sunlight": true
			},
			{
				"type": "color",
				"messageKey": "COLOUR_DATE",
				"label": "Date Colour",
				"defaultValue": "FFFFFF",
				"sunlight": true,
				"capabilities": ["NOT_BW"],
			},
			{
				"type": "color",
				"messageKey": "COLOUR_HOUR",
				"label": "Hour Colour",
				"defaultValue": "FFAA00",
				"sunlight": true,
				"capabilities": ["NOT_BW"],
			},
			{
				"type": "color",
				"messageKey": "COLOUR_MINUTE",
				"label": "Minute Colour",
				"defaultValue": "FFFFFF",
				"sunlight": true,
				"capabilities": ["NOT_BW"],
			},
			{
				"type": "color",
				"messageKey": "COLOUR_BLUETOOTH",
				"label": "Blutooth Disconnect Colour",
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
				"description": "Displays a symbol when the batery is 20% or less",
				"defaultValue": false,
			},
// 			{
// 				"type": "select",
// 				"messageKey": "SELECT_BATTERY_PERCENT",
// 				"defaultValue": "20",
// 				"label": "Low battery:",
// 				"options": [
// 					{ 
// 						"label": "10%",
// 						"value": "10" 
// 					},
// 					{ 
// 						"label": "20%",
// 						"value": "20" 
// 					},
// 					{ 
// 						"label": "30%",
// 						"value": "30" 
// 					},
// 					{ 
// 						"label": "40%",
// 						"value": "40" 
// 					},
// 					{ 
// 						"label": "50%",
// 						"value": "50" 
// 					},
// 				]
// 			},
// 			{
//  				"type": "select",
//  				"messageKey": "SELECT_BATTERY_COLOUR",
//  				"defaultValue": "white",
//  				"label": "Low battery colour:",
//  				"options": [
//  					{ 
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
				"type": "toggle",
				"messageKey": "TOGGLE_SUFFIX",
				"label": "Toggle suffix for date",
				"description": "You may need to wait a minute for the change to occur",
				"defaultValue": false,
			},

		]
	},
	{
		"type": "submit",
		"defaultValue": "Save"
	}
];

