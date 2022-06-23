import express, {NextFunction, Request, Response} from "express";
import createHttpError from "http-errors";
import ScannerModel from "../models/scanner";

const router = express.Router()

router.get('/scanners', async (req: Request, res: Response, next: NextFunction) => {
    // Get all the items in the database
    try {
        const queryResult = await ScannerModel.find().exec()
        res.send(queryResult)
    } catch (e) {
        next(e)
    }
})

router.delete('/scanners', async (req: Request, res: Response, next: NextFunction) => {
    // Delete all the items in the database
    try {
        const queryResult = await ScannerModel.deleteMany().exec()

        res.send(queryResult)
    } catch (e) {
        next(e)
    }
})

router.post('/scanner', async (req: Request, res: Response, next: NextFunction) => {
    // Create new item
    try {
        // Check whether the provided nfcId is not in the database
        const checkUnique = await ScannerModel.find({
            address: req.body.address,
        }).exec()

        if (!checkUnique) {
            throw new createHttpError.UnprocessableEntity("Address id should be unique")
        }

        // Create new model instance
        const instance = new ScannerModel({
            name: req.body.name,
            address: req.body.address,
        })

        await instance.save()

        res.send(instance)
    } catch (e) {
        next(e)
    }
})

router.get('/scanner/:address', async (req: Request, res: Response, next: NextFunction) => {
    // Get an item by its nfcId
    try {
        const instance = await ScannerModel.findOne({
            address: req.params.address,
        }).exec()

        if (!instance) {
            throw new createHttpError.UnprocessableEntity("Instance does not exist")
        }
        
        res.send(instance)
    } catch (e) {
        next(e)
    }
})

router.patch('/scanner/:address', async (req: Request, res: Response, next: NextFunction) => {
    // Update an item
    try {
        const instance = await ScannerModel.findOneAndUpdate({
            address: req.params.address,
        }, {
            address: req.body.address,
            name: req.body.name
        }).exec()

        if (!instance) {
            throw new createHttpError.UnprocessableEntity("Instance does not exist")
        }

        res.send(instance)
    } catch (e) {
        next(e)
    }
})

router.delete('/scanner/:address', async (req: Request, res: Response, next: NextFunction) => {
    // Delete an item
    try {
        const queryResult = await ScannerModel.deleteOne({
            address: req.params.address,
        }).exec()
        
        res.send(queryResult)
    } catch (e) {
        next(e)
    }
})

export default router