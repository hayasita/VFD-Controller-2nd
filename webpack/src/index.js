
// BootstrapのJavaScript側の機能を読み込む
import "bootstrap";

// スタイルシートを読み込む
import "./index.scss";

import '@riotjs/hot-reload'

import {StartModelSelection} from './view/appmain'

StartModelSelection()
