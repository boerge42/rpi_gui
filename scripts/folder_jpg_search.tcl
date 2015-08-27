#
# folder_jpg_search.tcl
# =====================
#   Uwe Berger, 2015
#
#
# * Durchsuchen eines Verzeichnisbaum ab $mp3_dir rekursiv
# * wenn jeweils eine jpg-Datei $fname_cd_cover vorhanden vorhanden,
#   dann Resize in Groesse $resize und entsprechende Umbenennung
#   der dabei entstehenden Bilddatei
#
# ---------
# Have fun!
#

set mp3_dir 				"/media/fritzbox/WD-MyPassport0810-01/mp3/"
set fname_cd_cover 			"folder.jpg"
set resize					"128x128"
set fname_cd_cover_resize 	"cd_cover_$resize.jpg"
set overwrite_all_resize	0
set count 0

# ***************************************
proc search_in_mp3_dir {dir} {
	global fname_cd_cover count fname_cd_cover_resize overwrite_all_resize
	
	if {[catch {set dir_list [glob -directory $dir -type d *]}]} {
		# wenn catch zuschlaegt, dann gibt es keine Unterverzeichnisse mehr...		
		incr count
		if {[catch {set cd_cover [glob -directory $dir $fname_cd_cover]}]} {
			puts "--> $dir --> $fname_cd_cover fehlt!"
		} else {
			set cd_cover [lindex $cd_cover 0]
			if {![file exists $dir/$fname_cd_cover_resize] || $overwrite_all_resize} {
				puts "convert $cd_cover -resize 128x128 $dir/$fname_cd_cover_resize"
				exec convert $cd_cover -resize 128x128 $dir/$fname_cd_cover_resize
			}
		}
	} else {
		foreach sub_dir $dir_list {
			search_in_mp3_dir $sub_dir
		}
	}
}


# ***************************************
# ***************************************
# ***************************************
puts "Beginn"
search_in_mp3_dir $mp3_dir
puts "Ende"
puts $count
