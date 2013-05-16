product application
id "application Description"
image sw
    id "Software"    version 1
    order 9999
    subsys base default
        id "Base Software"
        replaces self
        exp application.sw.base
    endsubsys
    subsys optional
        id "Optional Software"
        replaces self
        exp application.sw.optional
    endsubsys
endimage
image man
    id "Man Pages"
    version 1
    order 9999
    subsys manpages
        id "Man Pages"
        replaces self
        exp application.man.manpages
    endsubsys
    subsys relnotes
        id "Release Notes"
        replaces self
        exp application.man.relnotes
    endsubsys
    endimage
    endproduct
maint maint_application
endmaint
