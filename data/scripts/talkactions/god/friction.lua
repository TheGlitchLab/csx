local talk = TalkAction("/friction")

function talk.onSay(player, words, param)
    local newValue = not player:isIgnoringFriction()
    player:setIgnoreFriction(newValue)
    player:sendTextMessage(MESSAGE_LOOK, string.format("Your character %s.", newValue and "is super fast now" or "returns to normal speed"))
    return true
end

talk:separator(" ")
talk:groupType("god")
talk:register()
