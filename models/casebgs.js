let mongoose = require('mongoose')
let Schema = mongoose.Schema

let casebgsList = new Schema({
    "caseId": Number,
    "caseTitle": String,
    "keyword": String,
    "imgUrl": String
})

module.exports = mongoose.model('Casebg', casebgsList)