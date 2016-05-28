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
				"appKey": "KEY_COLOUR_BACKGROUND",
				"label": "Background Colour",
				"sunlight": true
			},
			{
				"type": "color",
				"appKey": "KEY_COLOUR_DATE",
				"label": "Date Colour",
				"sunlight": true
			},
			{
				"type": "color",
				"appKey": "KEY_COLOUR_HOUR",
				"label": "Hour Colour",
				"sunlight": true
			},
			{
				"type": "color",
				"appKey": "KEY_COLOUR_MINUTE",
				"label": "Minute Colour",
				"sunlight": true
			}
		]
	},
	{
		"type": "submit",
		"defaultValue": "Save"
	}
];

