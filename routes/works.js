var express = require('express');
var router = express.Router();
var mongoose = require('mongoose');
var Works = require('../models/works');

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

    let worksModel = Works.find(params).skip(skip).limit(pageSize)
    // sort Api 1是升序 -1是降序
    worksModel.sort({'postId': sort})
    Works.find({}, (err, doc) => {
        let docLength = doc.length
        worksModel.exec((err, doc) => {
            if (err) {
                res.json({
                    status: 0,
                    msg: err.message
                })
            } else {
                res.json({
                    status: 1,
                    msg: 'sucess',
                    result: {
                        count: docLength,
                        list: doc
                    }
                })
            }
        })
    })

})

module.exports = router;