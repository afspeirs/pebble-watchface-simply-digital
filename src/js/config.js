module.exports = [
	{ 
    	"type": "heading", 
    	"defaultValue": "Simply Digital",
		"size": 1
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
				"appKey": "COLOUR_BACKGROUND",
				"label": "Background Colour",
				"defaultValue": "000000",
				"sunlight": true
			},
			{
				"type": "color",
				"appKey": "COLOUR_DATE",
				"label": "Date Colour",
				"defaultValue": "FFFFFF",
				"sunlight": true
			},
			{
				"type": "color",
				"appKey": "COLOUR_HOUR",
				"label": "Hour Colour",
				"defaultValue": "FFAA00",
				"sunlight": true
			},
			{
				"type": "color",
				"appKey": "COLOUR_MINUTE",
				"label": "Minute Colour",
				"defaultValue": "FFFFFF",
				"sunlight": true
			}
		]
	},
// 	{
// 		"type": "section",
// 		"items": [
// 			{
// 				"type": "heading",
// 				"defaultValue": "Custom Date 1"
// 			},
// 			{
// 				"type": "input",
// 				"appKey": "CUSTOM1_DATE",
// 				"defaultValue": "",
// 				"label": "Date",
// 				"attributes": {
// 					"placeholder": "eg: 2512 (25th December)",
// 				}
// 			},
// 			{
// 				"type": "input",
// 				"appKey": "CUSTOM1_TOP",
// 				"defaultValue": "",
// 				"label": "Top Text",
// 				"attributes": {
// 					"placeholder": "eg: Happy",
// 				}
// 			},
// 			{
// 				"type": "input",
// 				"appKey": "CUSTOM1_BOTTOM",
// 				"defaultValue": "",
// 				"label": "Bottom Text",
// 				"attributes": {
// 					"placeholder": "eg: Birthday",
// 				}
// 			}
// 		]
// 	},	
	{
		"type": "section",
		"items": [
			{
				"type": "toggle",
				"appKey": "SUFFIX",
				"label": "Enable suffix for date",
				"description": "You will need to wait a minute for the change to occur",
				"defaultValue": false,
			}
		]
	},
	{
		"type": "submit",
		"defaultValue": "Save"
	}
];

