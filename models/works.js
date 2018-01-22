var mongoose = require('mongoose')
var Schema = mongoose.Schema

var workList = new Schema({
    "postId": Number,
    "postName": String,
    "education": String,
    "publishDate": String,
    "recruitNum": String,
    "serviceCondition": String,
    "workContent": String,
    "workPlace": String,
    "workYears": String,
    "postType": String,
    "workType": String
})

module.exports = mongoose.model('Work', workList)