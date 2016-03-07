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
			"appKey": "background",
			"defaultValue": "000000",
			"label": "Background Colour",
			"sunlight": true
			},
			{
			"type": "color",
			"appKey": "background",
			"defaultValue": "FF0000",
			"label": "Hour Colour",
			"sunlight": true
			},
			{
			"type": "color",
			"appKey": "background",
			"defaultValue": "FFFFFF",
			"label": "Minute Colour",
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
			"type": "toggle",
			"appKey": "enableAnimations",
			"label": "Enable Animations"
			},
			{
				"type": "input",
				"appKey": "email",
				"defaultValue": "",
				"label": "Email Address",
				"attributes": {
					"placeholder": "eg: name@domain.com",
					"limit": 10,
					"required": "required",
					"type": "email"
				}
			}
		]
	},
	{
		"type": "submit",
		"defaultValue": "Save"
	}
];

