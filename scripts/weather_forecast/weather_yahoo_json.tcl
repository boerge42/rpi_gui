# **********************************************
#  weather_yahoo_json.tcl
#  ======================
#     Uwe Berger; 2016
#
#  ---------
#  Have fun!
#
# https://query.yahooapis.com/v1/public/yql?q=select%20*%20from%20weather.forecast%20where%20location=%22GMBB0137%22%20and%20u=%22c%22&format=json
#
# **********************************************

package require Tcl 8.5
package require json
package require http

set url http://query.yahooapis.com/v1/public/yql?q=select%20*%20from%20weather.forecast%20where%20location=%22GMBB0137%22%20and%20u=%22c%22&format=json

# Dateiname fuer Outputs
source /home/bergeruw/work/weather_forecast/conf_fname.tcl

# **********************************************
proc ___getpage {url} {
	set fname data/yahoo_json.txt
	set fd [open $fname r] 
	set data [read $fd [file size $fname]]
	close $fd
	return $data
}

# **********************************************
proc getpage { url } {
	set token [::http::geturl $url]
	set data [::http::data $token]
	::http::cleanup $token          
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
proc convert_current_date {d} {
	return [clock format [clock scan $d] -format "%a, %d.%m.%Y, %H:%M"]
}

# **********************************************
proc convert_date {d} {
	return [clock format [clock scan $d] -format "%d.%m.%Y"]
}

# **********************************************
proc convert_time {d} {
	return [clock format [clock scan $d] -format "%H:%M"]
}

# ********************************
# ********************************
# ********************************

# Json-Daten holen und in ein Dict konvertieren
set d [::json::json2dict [getpage $url]]

# ist alles in query.results.channel drin...
set d [my_dict_get $d {query results channel}]

# libconf-Fomat generieren
set fd [open $conf_fname w]

# Ort-Informationen...
puts $fd "city = \"[my_dict_get $d {location city}]\""
puts $fd "longitude = \"[format "%.2f" [my_dict_get $d {item long}]]\""
puts $fd "latitude = \"[format "%.2f" [my_dict_get $d {item lat}]]\""
puts $fd "current_date = \"[convert_current_date [my_dict_get $d {lastBuildDate}]]\""
puts $fd "sunrise = \"[convert_time [my_dict_get $d {astronomy sunrise}]]\""
puts $fd "sunset = \"[convert_time [my_dict_get $d {astronomy sunset}]]\""

# Forecast-Werte sind in item.forecast als Array
set i 0
foreach f [my_dict_get $d {item forecast}] {
	puts $fd "forecast$i\_day = \"[my_dict_get $f day]\""
	puts $fd "forecast$i\_date = \"[convert_date [my_dict_get $f date]]\""
	puts $fd "forecast$i\_temp_low = \"[my_dict_get $f low]\""
	puts $fd "forecast$i\_temp_high = \"[my_dict_get $f high]\""
	puts $fd "forecast$i\_text = \"[my_dict_get $f text]\""
	puts $fd "forecast$i\_description = \"[my_dict_get $f text]\""
	puts $fd "forecast$i\_code = \"[my_dict_get $f code]\""
	puts $fd "forecast$i\_image = \"[my_dict_get $f code].jpg\""
	puts $fd "forecast$i\_icon = \"[my_dict_get $f code].png\""
	incr i
}
puts $fd "source = \"weather.yahoo.com\""

close $fd

# html-Datei erzeugen
source /home/bergeruw/work/weather_forecast/weather_forecast_html.tcl





