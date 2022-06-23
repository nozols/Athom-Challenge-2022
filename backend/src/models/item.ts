import {Schema, model, InferSchemaType} from "mongoose";

export const ItemSchema = new Schema({
    name: {
        type: String,
        required: false
    },
    price: {
        type: Number,
        required: false
    },
    nfcId: {
        type: String,
        required: true,
    },
})

const ItemModel = model('Item', ItemSchema)

export type ItemType = InferSchemaType<typeof ItemSchema>
export default ItemModel