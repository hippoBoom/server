let mongoose = require('mongoose')
let Schema = mongoose.Schema

let citemsList = new Schema({
    "prodId": Number,
    "prodTitle": String,
    "keyword": String,
    "imgUrl": String,
    "publishDate": String
})

module.exports = mongoose.model('Citem', citemsList)