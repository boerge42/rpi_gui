# **********************************************
#  weather_openweathermap_json.tcl
#  ===============================
#         Uwe Berger; 2016
#
#  Achtung: mind. Tcl-Version 8.5 wg. json
#  ---------
#  Have fun!
#
#
# **********************************************

package require Tcl 8.5
package require json
package require http


# Request-URL zusammenbauen
set id          id=2945358
set appid       appid=03d50d3f4ec97726d7993c4067743c95
set units       units=metric

set url http://api.openweathermap.org/data/2.5/forecast/daily?$id&$units&$appid

# Dateiname fuer Outputs
source /home/bergeruw/work/weather_forecast/conf_fname.tcl

# **********************************************
proc getpage { url } {
	set token [::http::geturl $url]
	set data [::http::data $token]
	::http::cleanup $token          
	return $data
}

# **********************************************
proc ___getpage {url} {
	set fname data/openweathermap_json.txt
	set fd [open $fname r] 
	set data [read $fd [file size $fname]]
	close $fd
	return $data
}

# ********************************
proc my_dict_get {d key_list} {
	foreach key $key_list {
		set d [dict get $d $key]
	}
	return $d
}


# **********************************************
proc format_icon_name {iname} {
	# meine Icons haben kein "-" im Dateinamen
	set iname [string map {- ""} $iname]
	# wenn an 4.Stelle ein ".", dann davor noch ein "d" einfuegen
	if {[string match . [string index $iname 3]]} {
		set iname [string map {. d.} $iname]
	}
	return $iname
}


# ********************************
# ********************************
# ********************************

# Json-Daten holen und in ein Dict konvertieren
set d [::json::json2dict [getpage $url]]

# libconf-Fomat generieren
set fd [open $conf_fname w]

# Ort-Informationen...
puts $fd "city = \"[my_dict_get $d {city name}]\""
puts $fd "longitude = \"[format "%.2f" [my_dict_get $d {city coord lon}]]\""
puts $fd "latitude = \"[format "%.2f" [my_dict_get $d {city coord lat}]]\""
puts $fd "current_date = \"[clock format [clock seconds] -format "%a, %d.%m.%Y, %H:%M"]\""

# Forecast...
set i 0
foreach f [my_dict_get $d list] {
	puts $fd "forecast$i\_day = \"[clock format [my_dict_get $f dt] -format "%a"]\""
	puts $fd "forecast$i\_date = \"[clock format [my_dict_get $f dt] -format "%d.%m.%Y"]\""
	puts $fd "forecast$i\_temp_low = \"[format "%.0f" [my_dict_get $f {temp min}]]\""
	puts $fd "forecast$i\_temp_high = \"[format "%.0f" [my_dict_get $f {temp max}]]\""
	set w [lindex [my_dict_get $f weather] 0]
	puts $fd "forecast$i\_text = \"[my_dict_get $w main]\""
	puts $fd "forecast$i\_description = \"[my_dict_get $w description]\""
	puts $fd "forecast$i\_code = \"[my_dict_get $w id]\""
	puts $fd "forecast$i\_image = \"[my_dict_get $w icon].jpg\""
	puts $fd "forecast$i\_icon = \"[format_icon_name [my_dict_get $w id].png]\""
	incr i
}
puts $fd "source = \"openweathermap.org\""

close $fd

# html-Datei erzeugen
source /home/bergeruw/work/weather_forecast/weather_forecast_html.tcl





