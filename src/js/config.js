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
				"type": "toggle",
				"messageKey": "SUFFIX",
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

