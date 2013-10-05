#source TemplaTcl.tcl

proc applytemplate {template} {
    set script ""
    set pos 0
    foreach pair [regexp -line -all -inline -indices {^%.*$} $template] {
        lassign $pair from to
        set str [string range $template $pos [expr {$from - 2}]]
		
		regsub -all {\[} $str  {\\[} str
        if {$str ne ""} {
            append script "puts \[" [list subst $str] \]\n
        }
        append script [string range $template [expr {$from + 1}] $to]\n
        set pos [expr {$to + 2}]
    }
    set str [string range $template $pos end]
	regsub -all {\[} $str  {\\[} str
    if {$str ne ""} {
        append script "puts -nonewline \[" [list subst $str] \]
    }
    uplevel 1 $script
}



proc readfile {filename} {
	set fsize [file size $filename]
	set fp [open $filename r]
	set data [read $fp $fsize]
	close $fp
	return $data
}

proc generateBooleanFunction {name call} {
	set text [readfile "function_bool.txt"]
	set text [string map [list "%name%" $name "%call%" $call] $text]
	puts $text
}

proc generateGeometryFunction {name call} {
	set text [readfile "function_geom.txt"]
	set text [string map [list "%name%" $name "%call%" $call] $text]
	puts $text
}

proc generateCompare {name call} {
	set text [readfile "compare.txt"]
	set text [string map [list "%name%" $name "%call%" $call] $text]
	puts $text
}

proc generateCompute {name call} {
	set text [readfile "compute.txt"]
	set text [string map [list "%name%" $name "%call%" $call] $text]
	puts $text
}

#proc generateCall {fparams} {
#	TemplaTcl::create tt
#	tt parseFile call.tmpl
#	tt setVar fparams $fparams
#	tt setVar cityList {Ragusa Ravenna Rieti Rimini Rome Rovigo}
#	puts [tt render]	
#}

proc generateCall {name call fparams} {
	set template [readfile "call.tmpl"]
	applytemplate $template 
}

proc generate {} {

	
	generateGeometryFunction "msudf_boundary"  "GEOSBoundary"
#   my_bool msudf_buffer_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
	generateGeometryFunction "msudf_centroid"  "GEOSGetCentroid"
	generateCompare "msudf_contains" "GEOSContains"
	generateGeometryFunction "msudf_convexHull" "GEOSConvexHull"
	generateCompare "msudf_crosses" "GEOSCrosses"
    generateCompute "msudf_difference" "GEOSDifference"
	generateCompare "msudf_disjoint" "GEOSDisjoint"
 	generateCompute "msudf_intersection" "GEOSIntersection"
 	generateCompare "msudf_intersects" "GEOSIntersects"
 	generateBooleanFunction "msudf_isEmpty"  "GEOSisEmpty"
 	generateBooleanFunction "msudf_isSimple" "GEOSisSimple"
    generateBooleanFunction "msudf_isRing"   "GEOSisRing"
 	generateBooleanFunction "msudf_isValid"  "GEOSisValid"
 	generateCompare "msudf_overlaps" "GEOSOverlaps"
 	generateCompare "msudf_touches" "GEOSTouches"
#   my_bool msudf_transform_init(UDF_INIT *initid,UDF_ARGS *args,char *message)

    generateCompute "msudf_symDifference" "GEOSSymDifference"
 	set fparams {{GEOSGeom geom1 STRING_RESULT ""} {double tolerance REAL_RESULT "*(double*)"}};
 	generateCall  "msudf_simplify"  "GEOSSimplify" $fparams
 	generateCall  "msudf_simplifyPreserveTopology" GEOSTopologyPreserveSimplify $fparams
	generateGeometryFunction "msudf_lineMerge"  "GEOSLineMerge"
 	set fparams {{GEOSGeom geom1 STRING_RESULT ""} {double start REAL_RESULT "*(double*)"} {double end REAL_RESULT "*(double*)"}}
 	generateCall  "msudf_lineSubstring"  "gu_substringLineGeom" $fparams
 	generateGeometryFunction "msudf_pointOnSurface" "GEOSPointOnSurface"
 	generateGeometryFunction "msudf_reverse" "gu_reverseGeom"
 	generateCompute "msudf_union" "GEOSUnion"
 	generateCompare "msudf_within" "GEOSWithin"

 
}

generate
