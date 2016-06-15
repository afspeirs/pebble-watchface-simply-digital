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
			},
			{
				"type": "color",
				"appKey": "COLOUR_BLUETOOTH",
				"label": "Blutooth Disconnect Colour",
				"defaultValue": "FF0000",
				"sunlight": true
			}
		]
	},	
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

