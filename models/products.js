var mongoose = require('mongoose')
var Schema = mongoose.Schema
var productList = new Schema({
    "prodId": Number,
    "series": String,
    "category": String,
    "models": String,
    "character": String,
    "size": String,
    "inputPort": String,
    "outputPort": String
})

module.exports = mongoose.model('Product', productList)