var mongoose = require('mongoose')
var Schema = mongoose.Schema
var faqList = new Schema({
    "faqId": Number,
    "question": String,
    "answer": String
})

module.exports = mongoose.model('Faq', faqList)