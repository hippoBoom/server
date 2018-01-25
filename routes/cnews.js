let express = require('express')
let router = express.Router()
let mongoose = require('mongoose')
let Cnews = require('../models/cnews')

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

    let cnewsModel = Cnews.find(params).skip(skip).limit(pageSize)
    cnewsModel.sort({'postId': sort})
    Cnews.find({}, (err, doc) => {
        let docLength = doc.length
        cnewsModel.exec((err, doc) => {
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
                        count: docLength,
                        list: doc
                    }
                })
            }
        })
    })

})

module.exports = router