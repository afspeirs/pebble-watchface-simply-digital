module.exports = [
	{ 
    	"type": "heading", 
    	"defaultValue": "Simply Digital",
		"size": 1
	}, 
	{ 
		"type": "text", 
		"defaultValue": "Example text item." 
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
				"appKey": "KEY_COLOUR_BACKGROUND",
				"label": "Background Colour",
				"defaultValue": "000000",
				"sunlight": true
			},
			{
				"type": "color",
				"appKey": "KEY_COLOUR_DATE",
				"label": "Date Colour",
				"defaultValue": "FFFFFF",
				"sunlight": true
			},
			{
				"type": "color",
				"appKey": "KEY_COLOUR_HOUR",
				"label": "Hour Colour",
				"defaultValue": "FF0000",
				"sunlight": true
			},
			{
				"type": "color",
				"appKey": "KEY_COLOUR_MINUTE",
				"label": "Minute Colour",
				"defaultValue": "FFFFFF",
				"sunlight": true
			}
		]
	},
	{
		"type": "submit",
		"defaultValue": "Save"
	}
];

