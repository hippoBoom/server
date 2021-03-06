var express = require('express');
var router = express.Router();
var mongoose = require('mongoose');
var Citems = require('../models/citems');

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
    let page = parseInt(req.param('page'))
    let pageSize = parseInt(req.param('pageSize'))
    let sort = parseInt(req.param('sort'))
    let skip = (page - 1) * pageSize

    let params = {}

    let citemsModel = Citems.find(params).skip(skip).limit(pageSize)
    citemsModel.sort({'prodId': sort})
    citemsModel.exec((err, doc) => {
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
