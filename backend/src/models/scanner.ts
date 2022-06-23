import {Schema, model, InferSchemaType} from "mongoose";

export const ScannerSchema = new Schema({
    name: {
        type: String,
        required: false,
    },
    address: {
        type: String,
        required: true,
    }
})

const ScannerModel = model('Scanner', ScannerSchema)

export type ScannerType = InferSchemaType<typeof ScannerSchema>
export default ScannerModel