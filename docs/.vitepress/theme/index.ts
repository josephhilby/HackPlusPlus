import DefaultTheme from 'vitepress/theme'
import type { EnhanceAppContext } from 'vitepress'

import './custom.css'
import NandGate from '../components/NandGate.vue'

export default {
    ...DefaultTheme,
    enhanceApp({ app }: EnhanceAppContext) {
        app.component('NandGate', NandGate)
    }
}
