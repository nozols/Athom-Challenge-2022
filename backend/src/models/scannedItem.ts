import {Schema, model, InferSchemaType} from "mongoose";

export const ScannedItemSchema = new Schema({
    item: {
        type: Schema.Types.ObjectId,
        ref: 'Item',
    },
    scanner: {
        type: Schema.Types.ObjectId,
        ref: 'Scanner',
    },
}, {
    timestamps: {
        createdAt: true,
        updatedAt: false,
    },
})

const ScannedItemModel = model('ScannedItem', ScannedItemSchema)

export type ScannedItemType = InferSchemaType<typeof ScannedItemSchema>
export default ScannedItemModel