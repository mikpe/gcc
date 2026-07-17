       *> { dg-do run }
       *> { dg-output-file "group2/DEBUG-ITEM_using_mh_identical.out" }
        identification division.
        program-id. dbgline.
        data division.
        working-storage section.
        01 var  picture is x(3).
        01 var2 picture is x(6).
        01 ws-debug-item.
            02 ws-debug-line picture is x(6) value "burp".
            02 filler picture is x value space.
            02 ws-debug-name picture is x(30).
            02 filler picture is x value space.
            02 ws-debug-sub-1 picture is s9999 
                                    sign is leading separate character.
            02 filler picture is x value space.
            02 ws-debug-sub-2 picture is s9999 
                                    sign is leading separate character.
            02 filler picture is x value space.
            02 ws-debug-sub-3 picture is s9999 
                                    sign is leading separate character.
            02 filler picture is x value space.
            02 ws-debug-contents picture is x(76).
        procedure division.
            display "defaults"
            display """" debug-line """"
            display """" debug-name """"
            display      debug-sub-1
            display      debug-sub-2
            display      debug-sub-3
            display """" debug-contents """"
            move  "aaLINE"               to ws-debug-line
            move  "bbNAME"               to ws-debug-name
            move  1234                 to ws-debug-sub-1
            move -2345                 to ws-debug-sub-2
            move  3456                 to ws-debug-sub-3
            move  "ccDebugging contents" to ws-debug-contents
            move ws-debug-line         to debug-line
            move ws-debug-name         to debug-name
            move ws-debug-sub-1        to debug-sub-1
            move ws-debug-sub-2        to debug-sub-2
            move ws-debug-sub-3        to debug-sub-3
            move ws-debug-contents     to debug-contents
            display "after the moves"
            display """" debug-line """"
            display """" debug-name """"
            display      debug-sub-1
            display      debug-sub-2
            display      debug-sub-3
            display """" debug-contents """"
           stop run.

