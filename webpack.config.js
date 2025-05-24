const path = require('path');
const webpack = require('webpack');
const TerserPlugin = require('terser-webpack-plugin');
const HtmlWebpackPlugin = require('html-webpack-plugin');

const config = {
    entry: {
        test: './index.js',
        demo: './src/demo.js'
    },
    mode: process.env.WEBPACK_SERVE ? 'development' : 'production',
    output: {
        filename: `[name].min.js`,
        library: {
            name: 'test',
            type: 'umd'
        }
    },
    optimization: {
        minimize: true,
        minimizer: [new TerserPlugin()]
    },
    resolve: {
        fallback: { path: false, fs: false, crypto: false, util: false }
    },
    module: {
        rules: [
            {
                test: /\.(woff|woff2|eot|ttf|png|svg)$/,
                type: 'asset/inline',
            }
        ],
    },
    devServer: {
        headers: {
            "Cross-Origin-Opener-Policy": "same-origin",
            "Cross-Origin-Embedder-Policy": "require-corp"
        },
        client: {
            overlay: {
                warnings: false,
                errors: true
            }
        },
        static: [
            {
                directory: path.join(__dirname, 'doc'),
                publicPath: '/doc',
            },
        ],
        proxy: [
            {
                context: ['/api'],
                target: 'http://localhost:8080/api',
                changeOrigin: true,
                pathRewrite: {
                    '^/api': ''
                }
            },
            {
                context: ['/audio'],
                target: 'http://localhost:59000',
                changeOrigin: true,
                // pathRewrite: {
                //     '^/audio':''
                //   }
            }
        ],
    },
    optimization: {
        minimizer: [
            new TerserPlugin({
                extractComments: false
            })
        ]
    },
    plugins: [
        new HtmlWebpackPlugin({
            chunks: ['demo'],
            template: "./src/demo.html",
        })
        // new webpack.DefinePlugin({
        //     WEBPACK_SERVE: JSON.stringify('hej')
        // })
        //    new BundleAnalyzerPlugin()
    ],
    devtool: 'eval-cheap-source-map',
}

module.exports = async (env, argv) => {

    // new webpack.DefinePlugin({
    //     MODULE_DIR: JSON.stringify(env.module_dir || '../..'),
    // })

    return config;
}