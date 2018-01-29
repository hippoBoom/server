let express = require('express')
let router = express.Router()
let mongoose = require('mongoose')
let Products = require('../models/products')

mongoose.connect('mongodb://47.104.156.124:36911/cdb')

mongoose.connection.on('connected', () => {
    console.log("MongoDB connected success.")
})
mongoose.connection.on('error', () => {
    console.log('MongoDB connected fail.')
})
mongoose.connection.on('disconnected', () => {
    console.log('MongoDB connected disconnected.')
})

router.get('/', (req, res, next) => {
    let series = req.param('series')
    let category = req.param('category')
    let params = {
        series: series,
        category: category
    }
    Products.find(params, (err, doc) => {
        if (err) {
            res.json({
                status: 0,
                msg: err.message
            })
        } else {
            res.json({
                status: 1,
                msg: 'success',
                result: {
                    count: doc.length,
                    list: doc
                }
            })
        }
    })
})

module.exports = router;
