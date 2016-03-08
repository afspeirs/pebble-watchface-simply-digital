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
				"appKey": "COLOUR_BACKGROUND",
				"label": "Background Colour",
				"defaultValue": "000000",
				"sunlight": true
			},
			{
				"type": "color",
				"appKey": "COLOUR_HOUR",
				"label": "Hour Colour",
				"defaultValue": "FF0000",
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
	{
		"type": "section",
		"items": [
			{
				"type": "heading",
				"defaultValue": "Custom Date 1"
			},
			{
				"type": "input",
				"appKey": "CUSTOM1_DATE",
				"defaultValue": "",
				"label": "Date",
				"attributes": {
					"placeholder": "eg: 2512",
					"limit": 4,
				}
			},
			{
				"type": "input",
				"appKey": "CUSTOM1_TOP",
				"defaultValue": "",
				"label": "Top Text",
				"attributes": {
					"placeholder": "eg: Happy",
					"limit": 16,
				}
			},
			{
				"type": "input",
				"appKey": "CUSTOM1_BOTTOM",
				"defaultValue": "",
				"label": "Bottom Text",
				"attributes": {
					"placeholder": "eg: Birthday",
					"limit": 16,
				}
			}
		]
	},
	{
		"type": "submit",
		"defaultValue": "Save"
	}
];

