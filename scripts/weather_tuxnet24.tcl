# **********************************************
#  weather_tuxnet24.tcl
#  ====================
#   Uwe Berger; 2015
#
#  Holt die aktuellen Wetterdaten und -vorhersagen
#  von http://weather.tuxnet24.de, und wandelt
#  diese in ins libconfig-Formate.
#
#  ---------
#  Have fun!
#
# **********************************************

package require http

set city_code	GMBB0137
set url			http://weather.tuxnet24.de/?id=$city_code
set conf_fname	/home/bergeruw/public_html/temp/weather_forecast.conf

# **********************************************
proc getpage { url } {
	set token [::http::geturl $url]
	set data [::http::data $token]
	::http::cleanup $token          
	return $data
}
 
# **********************************************
# **********************************************
# **********************************************

set d [split [getpage  $url] \n]

# libconf-Fomat generieren
set fd [open $conf_fname w]
foreach s $d {
	set s [split $s =]
	set key [string trim [lindex $s 0]]
	set value [string trim [lindex $s 1]]
	if {[string length $key]&&[string length $value]} {
		puts $fd "$key = \"$value\""
	}
}
close $fd
