import express, {NextFunction, Request, Response} from "express";
import createHttpError from "http-errors";
import ItemModel from "../models/item";

const router = express.Router()

router.get('/items', async (req: Request, res: Response, next: NextFunction) => {
    // Get all the items in the database
    try {
        const queryResult = await ItemModel.find().exec()

        res.send(queryResult)
    } catch (e) {
        next(e)
    }
})

router.delete('/items', async (req: Request, res: Response, next: NextFunction) => {
    // Delete all the items in the database
    try {
        const queryResult = await ItemModel.deleteMany().exec()

        res.send(queryResult)
    } catch (e) {
        next(e)
    }
})

router.post('/item', async (req: Request, res: Response, next: NextFunction) => {
    // Create new item
    try {
        // Check whether the provided nfcId is not in the database
        const checkUnique = await ItemModel.find({
            nfcId: req.body.nfcId,
        }).exec()

        if (checkUnique.length !== 0) {
            throw new createHttpError.UnprocessableEntity("NFC id should be unique")
        }
        
        // Create new model instance
        const instance = new ItemModel({
            name: req.body.name,
            nfcId: req.body.nfcId,
            price: req.body.price,
        })

        await instance.save()

        res.send(instance)
    } catch (e) {
        next(e)
    }
})

router.get('/item/:nfcId', async (req: Request, res: Response, next: NextFunction) => {
    // Get an item by its nfcId
    try {
        const instance = await ItemModel.findOne({
            nfcId: req.params.nfcId,
        }).exec()

        if (!instance) {
            throw new createHttpError.UnprocessableEntity("Instance does not exist")
        }
        
        res.send(instance)
    } catch (e) {
        next(e)
    }
})

router.patch('/item/:nfcId', async (req: Request, res: Response, next: NextFunction) => {
    // Update an item
    try {
        const instance = await ItemModel.findOneAndUpdate({
            nfcId: req.params.nfcId,
        }, {
            nfcId: req.body.nfcId,
            name: req.body.name,
            price: req.body.price,
        }).exec()

        if (!instance) {
            throw new createHttpError.UnprocessableEntity("Instance does not exist")
        }

        res.send(instance)
    } catch (e) {
        next(e)
    }
})

router.delete('/item/:nfcId', async (req: Request, res: Response, next: NextFunction) => {
    // Delete an item
    try {
        const queryResult = await ItemModel.deleteOne({
            nfcId: req.params.nfcId,
        }).exec()
        
        res.send(queryResult)
    } catch (e) {
        next(e)
    }
})

export default router