import DefaultTheme from 'vitepress/theme'
import type { EnhanceAppContext } from 'vitepress'

import './custom.css'
import * as components from '../components/index.js'

export default {
    ...DefaultTheme,
    enhanceApp({ app }: EnhanceAppContext) {
        Object.entries(components).forEach(([name, component]) => {
            app.component(name, component)
        })
    }
}
