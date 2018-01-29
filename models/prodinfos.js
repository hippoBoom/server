let mongoose = require('mongoose')
let Schema = mongoose.Schema
let infoList = new Schema({
    "series": String,
    "category": String,
    "intro": String,
    "character": String,
    "inSignal": String,
    "inFrequency": String,
    "inStandard": String,
    "inPort": String,
    "outSignal": String,
    "outFrequency": String,
    "outStandard": String,
    "outPort": String,
    "power": String,
    "pd": String,
    "ctrlMethod": String,
    "ipc": String,
    "ppc": String,
    "wholeDay": String,
    "define": String,
    "scp": String,
    "remote": String
})

module.exports = mongoose.model('Prodinfo', infoList)