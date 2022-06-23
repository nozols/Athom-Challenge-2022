import 'dotenv/config'
import express, {NextFunction, Request, Response} from 'express'
import { json } from 'body-parser'
import ItemRoutes from './routes/item'
import ScannerRoutes from './routes/scanner'
import ScanRoutes from './routes/scanneditem'
import mongoose from 'mongoose'
import createError from 'http-errors'

// Quick 'n dirty method for determining whether mongoosejs is ready for requests
let mongooseConnected = false

mongoose.connect(process.env.MONGOOSE_PATH ? process.env.MONGOOSE_PATH : 'mongodb://localhost/mongoose')
    .then(_connection => {
        console.log('Started mongoose')
        mongooseConnected = true
    })
    .catch(error => {
        console.error('Could not start mongoose!' )
        console.error(error)
    })

const app = express()
const router = express.Router()

app.use(json())
app.use(express.urlencoded({ extended: true }))

// While mongoose is not connected, always return a service unavailable error
app.use((req: Request, res: Response, next: NextFunction) => {
    console.log("Incoming request...", req.path)
    if (!mongooseConnected) {
        throw new createError.ServiceUnavailable('Mongoose not available')
    }

    next()
})

// Register routes
app.use(router)
app.use(ItemRoutes)
app.use(ScannerRoutes)
app.use(ScanRoutes)

// Basic error handler
app.use((err: any, req: Request, res: Response, next: NextFunction) => {
    console.error(err, err.stack)

    if (createError.isHttpError(err)) {
        res.status(err.statusCode).send(err)
    } else {
        res.status(500).send({
            type: 'Server error'
        })
    }    
})

// Start express
app.listen(process.env.EXPRESS_PORT, () => {
    console.log(`Server is listening on port ${process.env.EXPRESS_PORT}`)
})