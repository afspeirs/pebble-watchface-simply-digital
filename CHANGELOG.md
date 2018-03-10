## 0.21 [1b9b9df]
- Date is now configurable
## 0.24 [a367904]
- Added Aplite support
  - it is now just white, could possible pick more colours
- Fixed issue where a black screen appearing on first launch.
## 0.25 [fb8e31c]
- Added a suffix to the date (number) now has a suffix of st/nd/rd/th
- Removed support for aplite
  - It is erroring when losing bluetooth connection (will try to turn text red)
## 0.28 [f44b577]
- Added suffix toggle to the config
- Update clay to support new sdk
- Added option of enable/disable the suffix in the config
- Added Bebas Neue Regular for for the date.
  - Bebas Neue Bold is still used for the Hours and Minutes
## 0.29 [144eab4]
- Added option to configure the colour of the Hour and Minute, when the watch is no longer connected to your phone.
- Tweaked the code slightly to make it even more efficient :D
## 0.31 [3dd692c]
- Supports clay v1.0.0
- Support for Pebble Time Round
- Fixed an issue with the watchface crashing when configuring settings
## 0.32 [854d6df]
- Re-added aplite support
## 0.33 [eb34663]
- Added a low battery indicator when the watch hits 20%
  - Is toggleable in the config
## 0.35 [0825248]
- Added the option to configure the low battery icon percentage
- Fixed a bug where the battery icon wasn't showing at the time set in the settings page
  - It wasnt using the setting at all and defaulting back to 20%
- Further improved the way the suffix is shown
  - If the month is too long to be displayed, the month will be displayed by 3 chars rather than the full month (SEP instead of SEPTEMBER)
## 0.36 [721df13]
- The suffix will now update instantly after closing the settings page
- Fixed a bug where the suffix would only show "st".
- Alligned variable naming
## 0.38 [fd11026]
- Added support for custom text (enabled via check boxes in settings)
  - 3 options so far (New Years, Christmas and Halloween)
- Added support to select the format of the date (Day Month/Month Day)
- Added getSuffix() method
- Added a method to allow the customText to be called and therfore updated instantly
- Minor changes to settings page
## 0.39 [2cc3efb]
- Added black battery icon support
  - It will now, based on the background colour, select either the black or white battery icon to display
  - also fixes bug with battery not being transparent on Colour watches (Time and Round)
## 0.41 [1556472]
- Update battery icon formats to best (but the black image still needs png)
- Added Menu Image (the image in the wachface app on the watch)
## 0.43 [9a92c91]
- Optimisied inbox_received_handler() to call update_time() rather than duplicating code
## 0.44 [36c3cc0]
- Reduced watchface size by limiting the charactors Bold can use
- Repositioned the layout slightly
- Built using 4.0 sdk
  - Probably broke aplite support because of this
## 0.45 [c15993b]
- Added support for Quick View
## 0.46 [b463280]
- Fixed issue with bw config settings not loading
- Added Pebble 2 (Dorite) Support
## 0.48 [2763f25]
- Updated bluetooth configuration support.
  - Bluetooth colours now change the top and bottom date colours rather than the hour and minute colours
- Made date font smaller
  - 26pt vs 28pt
- Fixed issue with colour settings not being used correctly
## 0.49 [da9c64f]
- Removed configurable date format option.
- Optomised code
  - Fixed the date not centering properly
## 0.50 [ccfeefd]
- Removed several methods
  - Optomised code more as a result
- Fixed an issue where the bluetooth colours were not loading correcly on Black and White Pebbles
## 0.51 [4f9d6d0]
- Added support for other languages
- Added an option to toggle the bluetooth disconnection vibration (and colours) on/off
## 0.52 [bf6c518]
- Added option to toggle calendar week
- Removed Calendar week support for Chalk devices
## 0.53 [9f7951c]
- If you select no vibration it will change the colours correctly
## 0.54 [fb3fbb2]
- Changed how the battery icon && battery callback works
- Added more battery images
- You can now choose to display the battery icon based on a low battery percentage or show it all the time
- Simplified bluetooth callback
## 0.55 [083b51a]
- Reduced size of watchface
  - By limiting Bebas Neue Regular font to [0-9 a-zA-Z] it reduced the size of the watchface abit
## 0.56 [6dcce04]
- Rearanged configuration page
- Changed how config data is used
## 0.57 [da06ee6]
- Fixed the bluetooth vibration occuring on load and and when receiving config data.
  - By using a bool to decide if the watchface has properly started or not
## 0.58 [de3806f]
- Code optimisation
- Moved battery icon down 2px
- Optimisied how colours are assigned
## 0.59 [032f111]
- Updated layout of text for Pebble Round
## 0.60 [43d9dfb]
- Optimised code
  - The date is now updated every day rather than every minute
## 0.62 [dc840fc]
- Re-included support for custom text
- Changed how the text is displayed
  - back to how it was originaly as this change broke the custom text support
- Removed the option of having custom text shown
- Fixed the time not updating properly
- Changed the values at which the battery shows (11%,21% and 30%)
  - it still states 10, 20 and 30 in the settings
## 0.63 [0d09c28]
- Added Support for "Burns Night" and "Boxing Day"
- Tweaked the layout of the configuration page
- Changed Halloween to 3110
  - Halloween is broken for some reason
## 0.64 [467f43e]
- Changed how configuration settings are stored
  - It is now inline with pebbles recomended way
- Removed custom date options
  - As they were being a pain to set up correctly
- Removed custom date options
  - As they were being a pain to set up correctly
## 0.65.b1
- Based on 0.64.b1
- Removed one line of code that I think was breaking things
## 0.65 [197c4a9]
- Added a Version number to the config page
- Removed custom date options
  - As they were being a pain to set up correctly
- Optimised how the battery percentage is done
- Updated battery icon section of settings page
## 0.67 [95048b7]
- Minor text fixes
  - Changed language used in bluetooth section of settings page
- Fixed an issue where the battery icon was not showing correctly
  - Changed the stored variable to an int and parsed the string to an int before saving it
## 0.68 [ac55dd7]
- Added support for Pebble Time 2 watches (Emery)
- Made Orange (ChromeYellow) the default colour for the Hour (for watches that support colour)
## 0.69 [dda3296]
- Added toggle to vibrate during Quiet Time on Bluetooth disconnection
- Simplified how it checks for 12/24hr clock
- Made font smaller for Emery
## 0.70 [feb0c4a]
- Added Quiet Time icon
- Fixed issue with the icon not appearing other than on load of watchface
  - if you have it set up a scheduale it should just appear
- Fixed issue with it not updating the Quiet time icon when using the scheduale
## 0.71 [62c5c61]
- Realigned battery icon to better match the location of the Quiet Time icon
  - 2px right (when quiet time icon is not visible)
  - 3px left (when quiet time icon is visible)
## 0.72 [7297e22]
- Removed other battery icons
- Implemented showing of bluetooth icon when disconnected
## 1.1 [adf61e5]
- Removed support for Emery
- Simplified method to detect if the watch should vibrate when disconnected to phone
- Re-implimented custom dates
- Fixed rare issue of quiet time and battery icons overlapping
## 1.2 [e65594f]
- Added Christmas Eve as a custom date
## 1.3 [f2688e2]
- Fixed issue with Boxing day not appearing correctly on the 26th December
## 1.4 [94cb52e]
- Seperated update_time() into time and date methods
- Added a Power saver mode - Hides the time and date, until a flick/shake/tap of the watch
## 1.5 [9a22fa3]
- Added Bluetooth disconnection icon, and removed colour option.
## 1.6 [96c50a7]
- On days with a Custom Date enabled, eg Christmas Day or New Years day, will show the actual date on a flick/shake/tap of the watch
## 1.7 [82538b7]
- Fixed an issue where the background colour would not update if Power saver mode is enabled.
## 1.8 [39de642]
- Quiet Time logic improved
- Simplify methods
- Minor text amends
## 1.9 [8e2202b]
- Update images to work on Pebble 2
