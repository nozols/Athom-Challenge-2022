import express, {NextFunction, Request, Response} from "express";
import createHttpError from "http-errors";
import ItemModel from "../models/item";
import ScannedItemModel from "../models/scannedItem";
import ScannerModel from "../models/scanner";
import { sendToAllWebsockets } from "../socket";

const router = express.Router()

router.get('/scan', async (req: Request, res: Response, next: NextFunction) => {
    // Get all scanned items with the scanner and item populated
    try {
        const scans = await ScannedItemModel.find().populate('item').populate('scanner').exec()

        res.send(scans)
    } catch (e) {
        next(e)
    }
})

router.post('/scan', async (req: Request, res: Response, next: NextFunction) => {
    // Add a scanned item to the database and return the item that was scanned
    try {
        const itemInstance = await ItemModel.findOne({
            nfcId: req.body.nfcId
        }).exec()

        if (!itemInstance || itemInstance.name === undefined) {
            // If this item was never scanned before, create it in the database.
            // This is done so a name can be assigned later in the frontend application
            if (!itemInstance) {
                const newItem = new ItemModel({
                    nfcId: req.body.nfcId
                });

                await newItem.save();
            }

            throw new createHttpError.UnprocessableEntity("Could not find item")
        }

        let scannerInstance = await ScannerModel.findOne({
            address: req.body.address
        }).exec()

        if (!scannerInstance) {
            console.log("Unregistered scanner! Creating...");

            scannerInstance = new ScannerModel({
                address: req.body.address,
                name: "Scanner"
            });

            await scannerInstance.save();
        }

        const instance = new ScannedItemModel({
            item: itemInstance,
            scanner: scannerInstance,
        })

        await instance.save()
        
        res.send(itemInstance)

        sendToAllWebsockets('new-scan')
    } catch (e) {
        next(e)
    }
})

router.delete('/scan', async (req: Request, res: Response, next: NextFunction) => {
    // Delete all the scanned items in the database
    try {
        const queryResult = await ScannedItemModel.deleteMany().exec()

        res.send(queryResult)
    } catch (e) {
        next(e)
    }
})

router.delete('/scan/:id', async (req: Request, res: Response, next: NextFunction) => {
    // Delete the item scan
    try {
        const queryResult = await ScannedItemModel.findByIdAndDelete(req.params.id).exec()

        res.send(queryResult)
    } catch (e) {
        next(e)
    }
})

export default router