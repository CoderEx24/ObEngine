Define Flag (Settings);

GameConfig:
    mapName:"poly2.map.msd"
	antiAliasing:True
    depthOfField:True
    scrollSensibility:1
    framerateLimit:True
    framerateTarget:144
    vsync:True
    
KeyBinding:
    @game
        LPaddleUp:"A"
        LPaddleDown:"Q"
        RPaddleUp:"Up"
        RPaddleDown:"Down"
    @mapEditor
        SpriteMode:"W"
        ObjectMode:"X"
        CollisionMode:"C"
        CamFixed:"F10"
        CamFollow:"F11"
        CamZone:"F12"
        CamUp:"Z"
        CamDown:"S"
        CamLeft:"Q"
        CamRight:"D"
        CamDash:"LShift"
        RotateLeft:"Divide"
        RotateRight:"Multiply"
        ScaleInc:"Add"
        ScaleDec:"Substract"
        ZInc:"NumPad8"
        ZDec:"NumPad2"
        LayerInc:"NumPad6"
        LayerDec:"NumPad4"
        CancelOffset:"O"
        DeleteSprite:"Delete"
        GetCursorPos:"N"
        MagnetizeCursor:"M"
        MagnetizeUp:"Up"
        MagnetizeRight:"Right"
        MagnetizeDown:"Down"
        MagnetizeLeft:"Left"
    
Developpement:
    showCharacter:True
    showLevelSprites:True
    showOverlay:True
    showCursor:True
    showCollisions:False
    showFPS:True
    COMM:"COM9"