const char* htmlContent = R"=====(
<!DOCTYPE html>
<html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>TRITECH CNC</title>
        <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/normalize/8.0.1/normalize.min.css">
        <link rel="stylesheet" href="./css/base.css">
        <link rel="stylesheet" href="./css/main.css">
        <link rel="stylesheet" href="./fonts/fontawesome-free-6.5.1-web/css/all.min.css">
        <link rel="preconnect" href="https://fonts.googleapis.com">
        <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
        <link href="https://fonts.googleapis.com/css2?family=Roboto:wght@300;400;500;700&display=swap" rel="stylesheet">
        <link rel="stylesheet" href="https://fonts.googleapis.com/icon?family=Material+Icons">
    </head>
    <style>
        :root {
            --primary-color:#4e8bf4;
            --white-color: #fff;
            --black-color: #000;
            --text-color: #333;
            --border-color: #dbdbdb;

            --header-height: 120px;
            --navbar-height: 34px;
            --header-with-search-height: calc(var(--header-height) - var(--navbar-height));
        }

        * {
            box-sizing: inherit;
        }

        html {
            font-size: 62.5%;
            line-height: 1.6rem;
            font-family: "Roboto", sans-serif;
            box-sizing: border-box;
        }

        /* Responsive */
        .grid {
            width: 1400px;
            max-width: 100%;
            margin: 0 auto;
        }

        .grid__full-width {
            width: 100%;
        }

        .grid__row {
            display: flex;
            flex-wrap: wrap;
            margin-left: -12px;
            margin-right: -12px;
        }

        /* Test */
        .grid__column-2 {
            padding-left: 12px;
            padding-right: 12px;
            width: 16.666667%;
        }

        .grid__column-5 {
            padding-left: 12px;
            padding-right: 12px;
            width: 41.666667%;
        }

        .grid__column-10 {
            padding-left: 12px;
            padding-right: 12px;
            width: 83.333333%;
        }

        /* Animation */
        @keyframes fadeIn {
            from {
                opacity: 0;
            }
            to {
                opacity: 1;
            }
        }

        @keyframes growth {
            from {
                transform: scale(var(--growth-from));
            }
            to {
                transform: scale(var(--growth-to));
            }
        }

        .modal {
            position: fixed;
            top: 0;
            right: 0;
            bottom: 0;
            left: 0;
            display: flex;
            justify-content: center;
            align-items: center;
            animation: fadeIn linear 0.1s;
        }

        .modal__overlay {
            position: absolute;
            width: 100%;
            height: 100%;
            background-color: rgba(0, 0, 0, 0.4);
        }

        .modal__body {
            --growth-from: 0.7;
            --growth-to: 1;
            position: relative;
            z-index: 1;
            animation: growth linear 0.1s;
        }

        /* Button style */
        .btn {
            min-width: 124px;
            height: 34px;
            text-decoration: none;
            border: none;
            border-radius: 2px;
            font-size: 1.4rem;
            padding: 0 12px;
            outline: none;
            cursor: pointer;
            background-color: var(--white-color);
            color: var(--text-color);
            display: inline-flex;
            align-items: center;
            justify-content: center;
            line-height: 1.6rem;
        }

        .btn.btn--normal:hover {
            background-color: rgba(0, 0, 0, 0.2);
        }

        .btn.btn--size-s {
            height: 32px;
            font-size: 10px;
        }

        .btn.btn--primary {
            color: var(--white-color);
            background-color: var(--primary-color);
        }

        .btn.btn--disabled {
            color: #949494;
            cursor: default;
            background-color: #c3c3c3;
        }

        /* Selection */

        .select-input{
            position: relative;
            min-width: 200px;
            height: 34px;
            padding: 0 12px;
            border-radius: 2px;
            background-color: var(--white-color);
            display: flex;
            align-items: center;
            justify-content: space-between;
        }

        .select-input:hover .select-input__list {
            display: block;
        }

        .select-input__label {
            font-size: 1.4rem;
        }

        .select-input__icon {
            font-size: 1.4rem;
            color: rgb(131, 131, 131);
            position: relative;
            top: -1px;
        }

        .select-input:hover .select-input__list {
            display: block;
        }

        .select-input__list {
            position: absolute;
            left: 0;
            right: 0;
            top: 25px;
            border-radius: 2px;
            background-color: var(--white-color);
            padding: 8px 16px;
            list-style: none;
            display: none;
        }

        .select-input__link {
            font-size: 1.4rem;
            color: var(--text-color);
            text-decoration: none;
            display: block;
            padding: 6px 0;
        }

        .select-input__link:hover {
            color: var(--primary-color);
        }

        .header {
            height: var(--header-height);
            background-image: linear-gradient(0, #058da5, #4e8bf4);
        }

        .header_navbar {
            display: flex;
            justify-content: space-between;
            height: var(--navbar-height);
        }

        .header__navbar-list {
            list-style: none;
            padding-left: 0;
            margin-top: 4px 0 0 0;
            display: flex;
        }

        .header__navbar-item {
            margin: 0 8px;
            position: relative;
            min-height: 26px;
        }

        .header__navbar-user {
            display: flex;
            justify-items: center;
            position: relative;
        }

        .header__navbar-user-img {
            width: 22px;
            height: 22px;
            border-radius: 50%;
            border: 1px solid rgba(0, 0, 0, 0, 0.1);
        }

        .header__navbar-user-name {
            margin-left: 4px;
            font-size: 1.4rem;
            font-weight: 400;
        }

        .header__navbar-user:hover .header__navbar-user-menu {
            display: block;
        }

        .header__navbar-user-menu {
            position: absolute;
            z-index: 1;
            padding-left: 0;
            margin-top: 6px;
            top: 100%;
            right: 0;
            background-color: var(--white-color);
            border-radius: 2px;
            width: 160px;
            list-style: none;
            box-shadow: 1px 3px #c4c4c4;
            display: none;
        }

        .header__navbar-user-menu::before {
            content: "";
            border-width: 15px 20px;
            border-style: solid;
            border-color: transparent transparent var(--white-color) transparent;
            position: absolute;
            right: 5px;
            top: -20px;
        }

        .header__navbar-user-menu:after {
            content: "";
            display: block;
            position: absolute;
            top: -8px;
            right: 0;
            width: 56%;
            height: 11px;
        }

        /* .header__navbar-user-item {

        } */

        .header__navbar-user-item--separate {
            border-top: 1px solid rgba(0, 0, 0, 0.1);
        }

        .header__navbar-user-item a {
            text-decoration: none;
            color: var(--text-color);
            font-size: 1.4rem;
            padding: 8px 16px;
            display: block;
        }

        .header__navbar-user-item a:first-child {
            border-top-left-radius: 2px;
            border-top-right-radius: 2px;
        }

        .header__navbar-user-item a:last-child {
            border-bottom-left-radius: 2px;
            border-bottom-right-radius: 2px;
        }

        .header__navbar-user-item a:hover {
            background-color: #f3f3f3;
        }

        .header__navbar-item--has-qr:hover .header__qr {
            display: block;
        }

        .header__navbar-item--has-notify:hover .header__notify {
            display: block;
        }

        .header__navbar-item, 
        .header__navbar-item-link {
            display: inline-block;
            font-size: 1.3rem;
            color: var(--white-color);
            text-decoration: none;
            font-weight: 300;
        }

        .header__navbar-item, 
        .header__navbar-item-link,
        .header__navbar-icon-link {
            display: flex;
            align-items: center;
        } 

        .header__navbar-item:hover,
        .header__navbar-icon-link:hover,
        .header__navbar-item-link:hover {
            cursor: pointer;
            color: rgba(255, 255, 255, 0.7);
        }

        .header__navbar-item--strong {
            font-weight: 400;
        }

        .header__navbar-item--separate::after{
            content: "";
            display: block;
            position: absolute;
            border-left: 1px solid #c6eff1;
            height: 10px;
            right: -10px;
            top: 50%;
            transform: translateY(-50%);
        }

        .header__navbar-icon-link {
            color: var(--white-color);
            text-decoration: none;
        }

        .header__navbar-icon-link:nth-child(2) {
            margin-left: 3px;
        }

        .header__navbar-icon {
            font-size: 1.6rem;
            margin: 0 4px
        }

        .header__navbar-title--no-pointer {
            cursor: text;
            color: var(--white-color);
        }

        /* Header QR code */
        .header__qr {
            width: 200px;
            background-color: var(--white-color);
            position: absolute;
            left: 0;
            top: 120%;
            padding: 8px;
            border-radius: 2px;
            display: none;
            animation: fadeIn ease-in 0.2s;
            z-index: 1;
        }

        .header__qr::before {
            position: absolute;
            width: 100%;
            height: 20px;
            top: -16px;
            left: 0;
            content: "";
            display: block;
        }

        .header__qr-img {
            width: 100%;
        }

        .header__qr-apps {
            display: flex;
            justify-content: space-between;
        }

        .header__qr-link:nth-child(1) {
            margin-left: 9px;
            position: relative;
            bottom: 10px;
        }

        .header__qr-link:nth-child(2) {
            margin-right: 11px;
            position: relative;
            bottom: 10px;
        }

        .header__qr-download-img {
            height: 28px;
        }

        /* Header notification */
        .header__notify {
            position: absolute;
            z-index: 1;
            top: 118%;
            right: 0;
            width: 404px;
            border-radius: 2px;
            border: 1px solid rgba(0, 0, 0, 0, 0.1);
            background-color: var(--white-color);
            cursor: default;
            transform-origin: calc(100% - 20px) top;
            animation: headerNotifyGrowth ease-in 0.2s;
            -webkit-animation: headerNotifyGrowth ease-in 0.2s;
            will-change: opacity, transform;
            display: none;
        }

        .header__notify::before {
            content: "";
            border-width: 20px 20px;
            border-style: solid;
            border-color: transparent transparent var(--white-color) transparent;
            position: absolute;
            right: 15px;
            top: -28px;
        }

        .header__notify::after {
            content: "";
            display: block;
            position: absolute;
            right: 0;
            top: -16px;
            width: 90px;
            height: 20px;
        }

        @keyframes headerNotifyGrowth {
            from {
                opacity: 0;
                transform: scale(0);
            }
            to {
                opacity: 1;
                transform: scale(1);
            }
        }

        .header__notify-header {
            height: 40px;
            background-color: var(--white-color);
            border-radius: 2px;
        }

        .header__notify-header h3 {
            color: #999;
            margin: 0 0 0 12px;
            font-weight: 400;
            font-size: 1.4rem;
            line-height: 40px;
            user-select: none;
        }

        .header__notify-list{
            padding-left: 0;
        }

        .header__notify-item{
            display: flex;
        }

        .header__notify-item:hover {
            background-color: rgba(78, 139, 244, 0.09);
            
        }

        .header__notify-item--viewed {
            background-color: #f7f7f7;
        }

        .header__notify-link{
            display: flex;
            width: 100%;
            padding: 12px;
            text-decoration: none;
            align-items: center;
        }

        .header__notify-img{
            width: 40px;
            height: 40px;
            margin: 2px;
            border: 1px solid #E8E8E8;
        }

        .header__notify-info{
            margin-left: 12px;
        }

        .header__notify-name{
            display: block;
            font-size: 1.4rem;
            color: var(--text-color);
            font-weight: 500;
        }

        .header__notify-description{
            display: block;
            font-size: 1.2rem;
            color: var(--text-color);
        }

        .header__notify-footer {
            border-radius: 2px;
        }

        .header__notify-footer-btn {
            background-color: var(--white-color);
            text-decoration: none;
            color: var(--text-color);
            width: 100%;
            padding: 8px 0;
            text-align: center;
            font-size: 1.4rem;
            font-weight: 400;
            display: block;
        }

        .header__notify-footer-btn:hover {
            color: var(--primary-color);
            background-color: rgb(187, 187, 187, 0.02);
        }

        /* Auth Form */

        .auth-form {
            width: 500px;
            background-color: var(--white-color);
            border-radius: 5px;
            overflow: hidden;
        }

        .auth-form__container {
            padding: 0 32px;
        }

        .auth-form__header {
            display: flex;
            align-items: center;
            justify-content: space-between;
            padding: 0 12px;
            margin-top: 10px;
        }

        .auth-form__heading {
            font-size: 2.2rem;
            font-weight: 400;
            color: var(--text-color);
        }

        .auth-form__switch-btn {
            font-size: 1.6rem;
            color: var(--primary-color);
            font-weight: 400;
            cursor: pointer;
        }

        .auth-form__form {
            margin-top: 16px;
        }

        .auth-form__input {
            width: 100%;
            height: 40px;
            margin-top: 16px;
            padding: 0 12px;
            font-size: 1.4rem;
            border-radius: 2px;
            border: 1px solid var(--border-color);
            outline: none;
        }

        .auth-form__input:focus {
            border-color: #777;
        }

        .auth-form__aside {
            margin-top: 16px;
        }

        .auth-form__policy-text {
            font-size: 1.2rem;
            line-height: 1.8rem;
            text-align: center;
            padding: 0 12px;
        }

        .auth-form__text-link {
            color: var(--primary-color);
            text-decoration: none;
            font-weight: 400;
        }

        .auth-form__controls {
            margin-top: 40px;
            display: flex;
            justify-content: flex-end;
        }

        .auth-form__controls-return {
            margin-right: 8px;
        }

        .auth-form__socials {
            background-color: #d0d0d0;
            padding: 16px 36px;
            display: flex;
            justify-content: space-between;
            margin-top: 22px;
        }

        .auth-form__socials-icon {
            font-size: 1.8rem;
        }

        .auth-form__socials--facebook {
            background-color: #3A5A98;
            color: var(--white-color);
        }

        .auth-form__socials--facebook
        .auth-form__socials-icon {
            color: var(--white-color);
        }

        .auth-form__socials-title {
            margin: 0 36px;
        }

        .auth-form__socials--google{
            color: #666;
            background-color: var(--white-color);
        }

        .auth-form__socials-icon-google {
            width: 18px;
            object-fit: contain;
        }

        .auth-form__socials-title-google {
            color: var(--text-color);
        }

        .auth--form__help {
            display: flex;
            justify-content: flex-end;
        }

        .auth-form__help-link {
            font-size: 1.4rem;
            text-decoration: none;
            color: #7e7e7e;
        }

        .auth-form__help-separate {
            display: block;
            height: 22px;
            margin: 0 16px 0;
            border-left: 1px solid var(--border-color);
        }

        .auth-form__help-forgot {
            color: var(--primary-color);
        }

        /* Header with search */
        .header-with-search {
            height: var(--header-with-search-height);
            display: flex;
            align-items: center;
            margin: 0 8px;
        }

        .header__logo {
            width: 200px;
        }

        .header__logo-link {
            color: transparent;
            text-decoration: none;
        }

        .header__logo-img {
            width: 150px;
            color: var(--white-color);
        }

        .header__search {
            flex: 1;
            height: 40px;
            border-radius: 2px;
            background-color: var(--white-color);
            display: flex;
            align-items: center;
        }

        .header__menu {
            width: 150px;
            text-align: center;
            margin-left: 10px;
        }

        .header__menu-wrap {
            position: relative;
            display: inline-block;
            padding: 0 12px;
            cursor: pointer;
        }

        .header__menu-icon {
            color: var(--white-color);
            font-size: 2.4rem;
            margin-top: 2px;
        }

        .header__menu-wrap:hover .header__menu-list {
            display: block;
        }

        .header__menu-list {
            position: absolute;
            top: calc(100% + 8px);
            right: -8px;
            background-color: var(--white-color);
            width: 400px;
            border-radius: 2px;
            box-shadow: 0 2px 10px #c4c4c4;
            display: none; 
            cursor: default;
        }

        .header__menu-heading {
            text-align: left;
            margin: 8px 0 8px 12px;
            font-size: 1.4rem;
            color: #999;
        }

        .header__menu-list-iteam {
            padding-left: 0;
            list-style: none;
        }

        .header__menu-iteam {
            display: flex;
            align-items: center;
        }

        .header__menu-iteam:hover {
            background-color: rgba(78, 139, 244, 0.09);
        }

        .header__menu-img {
            width: 42px;
            height: 42px;
            margin: 12px;
            border: 1px solid #E8E8E8;
        }

        .header__menu-iteam-info {
            width: 100%;
            margin-right: 12px;
        }

        .header__menu-iteam-head {
            display: flex;
            align-items: center;
            justify-content: space-between;
        }

        .header__menu-iteam-name {
            font-size: 1.4rem;
            font-weight: 500;
            color: var(--text-color);
            margin: 0;
        }

        .header__menu-iteam-state-wrap {
            display: flex;
            align-items: center;
        }

        .header__menu-iteam-state {
            font-size: 1.4rem;
            font-weight: 400;
            color: var(--primary-color);
        }

        .header__menu-iteam-icon-state {
            font-size: 0.9rem;
            margin: 0 15px;
            color: #08ad08;
        }

        .header__menu-iteam-body {
            display: flex;
            justify-content: space-between;
            
        }

        .header__menu-iteam-description {
            color: #756F6E;
            font-size: 1.2rem;
            font-weight: 400;
        }

        .header__menu-iteam-stop {
            color: var(--text-color);
            font-size: 1.4rem;
            margin-right: 48px;
        }

        .header__menu-iteam-stop:hover {
            color: var(--primary-color);
            cursor: pointer;
        }

        .header__menu-view-menu {
            float: right;
            margin: 0 12px 12px 0;
        }

        .header__menu-view-menu:hover {
            background-color: #6a9ffc;
        }

        .header__menu-list::after {
            cursor: pointer;
            content: "";
            position: absolute;
            right: 4px;
            top: -28px;
            border-width: 20px 26px;
            border-style: solid;
            border-color: transparent transparent var(--white-color);
        }

        .header__menu-list--no-menu {
            padding: 24px 0;
        }

        .header__menu-list--no-menu .header__menu-no-menu-img,
        .header__menu-list--no-menu .header__menu-list-no-menu-msg {
            display: block;
            margin-left: auto;
            margin-right: auto;
        }

        .header__menu-no-menu-img {
            width: 60%;
            display: none;      
        }

        .header__menu-list-no-menu-msg{
            display: none;
            font-size: 1.4rem;
            margin-top: 14px;
            color: var(--text-color);
        }

        .header__menu-notice {
            position: absolute;
            padding: 1px 7px;
            top: -4px;
            right: 2px;
            font-size: 1.4rem;
            line-height: 1.4rem;
            border-radius: 10px;
            border: 2px solid #528df5;
            color: var(--primary-color);
            background-color: var(--white-color);
        }

        .header__search-input-wrap {
            flex: 1;
            height: 100%;
            position: relative;
        }

        .header__search-input {
            width: 100%;
            height: 100%;
            border: none;
            outline: none;
            font-size: 1.5rem;
            color: var(--text-color);
            padding: 0 16px;
            border-radius: 2px;
        }

        .header__search-select {
            border-left: 1px solid #c4c4c4;
            padding-left: 16px;
            position: relative;
            cursor: pointer;
        }

        .header__search-select-lable {
            font-size: 1.4rem;
            color: var(--text-color);
        }

        .header__search-select-icon {
            font-size: 1.4rem;
            color: rgb(131, 131, 131);
            margin: 0 16px 0 8px;
        }

        .header__search-btn {
            background-color: var(--primary-color);
            border: none;
            height: 34px;
            width: 64px;
            border-radius: 2px;
            margin-right: 3px;
            outline: none;
        }

        .header__search-btn:hover {
            cursor: pointer;
            background-color: #6a9ffc;
        }

        .header__search-select:hover .header__search-option {
            display: block;
        }

        .header__search-btn-icon {
            font-size: 1.6rem;
            color: var(--white-color);
        }

        .header__search-option {
            position: absolute;
            right: 0;
            top: calc(100% - 2px);
            width: 130px;
            list-style: none;
            padding-left: 0;
            box-shadow: 0 1px 3px #e0e0e0;
            border-radius: 2px;
            display: none;
            margin-top: 15px;
            animation: fadeIn ease-in 0.1s;
        }

        .header__search-option::after {
            content: "";
            display: block;
            width: 100%;
            height: 14px;
            position: absolute;
            top:-14px;
            left: 0;
        }

        .header__search-option-item {
            background-color: var(--white-color);
            padding: 8px 8px;
        }

        .header__search-option-item:first-child {
            border-top-left-radius: 2px;
            border-top-right-radius: 2px;
        }

        .header__search-option-item:last-child {
            border-bottom-left-radius: 2px;
            border-bottom-right-radius: 2px;
        }

        .header__search-option-item:hover {
            background-color: #f3f3f3;
            cursor: pointer;
        }

        .header__search-option-item span {
            font-size: 1.4rem;
            color: var(--text-color);
            margin-left: 12px;
        }

        .header__search-option-item i {
            font-size: 1.2rem;
            color: var(--primary-color);
            margin-left: 12px;
            display: none;
        }

        .header__search-option-item--active i {
            display: inline-block;
        }

        .header__search-input:focus ~ .header-search-history {
            display: block;
        }

        /* Search history */
        .header-search-history {
            position: absolute;
            top: calc(100% + 2px);
            left: 0;
            width: calc(100% - 16px);
            background-color: var(--white-color);
            border-radius: 2px;
            box-shadow: 0 1px 5px rgb(187, 187, 187);
            display: none;
            overflow: hidden;
        }

        .header-search-history-heading {
            margin: 6px 16px;
            font-size: 1.4rem;
            color: #999;
            font-weight: 400;
        }

        .header-search-history-list {
            list-style: none;
            padding-left: 0;
            margin-bottom: 0;
            margin: 6px 0 0;
        }

        .header-search-history-item {
            height: 38px;
            padding: 0 16px;
        }

        .header-search-history-item:hover {
            background-color: #f3f3f3;
        }

        .header-search-history-item a {
            text-decoration: none;
            color: var(--text-color);
            font-size: 1.2rem;
            line-height: 38px;
            display: block;
        }

        /* App container */
        .app__container {
            background-color: #F5F5F5;
        }

        .app__content {
            padding-top: 36px;
        }

        .category {
            background-color: var(--white-color);
            border-radius: 2px;
        }

        .category__heading {
            color: var(--text-color);
            font-size: 1.6rem;
            padding: 12px 16px;
            border-bottom: 1px solid rgb(187, 187, 187, 0.2);
            margin-top: 0;
        }

        .category__heading-icon {
            font-size: 1.4rem;
            margin-right: 4px;
            position: relative;
            top: -1px;
        }

        .category-list {
            padding:0 0 8px 0;
            list-style: none;
            margin-left: 9px;
        }

        /* .category-item {

        } */

        .category-item--active .category-item__link {
            color: var(--primary-color);
        }

        .category-item--active .category-item__link::before {
            content: "";
            top: 50%;
            left: 7px;
            position: absolute;
            border:4px solid;
            transform: translateY(calc(-50% - 1px));
            border-color: transparent transparent transparent var(--primary-color);
        }

        .category-item__link {
            position: relative;
            font-size: 1.5rem;
            color: var(--text-color);
            text-decoration: none;
            padding: 4px 16px;
            display: block;
            transition: right linear 0.05s;
            right: 0;
        }

        .category-item__link:hover {
            right: -4px;
            color: var(--primary-color);
        }

        /* HOME sort + filter */

        .home-filter {
            background-color: rgba(39, 169, 216, 0.2);
            align-items: center;
            padding: 12px 22px;
            border-radius: 2px;
            display: flex;
        }
        .home-filter__label{
            font-size: 1.4rem;
            color: #555;
            margin-right: 16px;

        }

        .home-filter__btn {
            min-width: 90px;
            margin-right: 12px;
        }

        .home-filter__page {
            display: flex;
            align-items: center;
            margin-left: auto;
        }

        .home-filter__page-num {
            font-size: 1.5rem;
            color: var(--text-color);
            margin-right: 22px;
        }

        .home-fileter__page-current {
            color: var(--primary-color);
        }

        .home-filter__page-control{
            border-radius: 2px;
            overflow: hidden;
            display: flex;
            width: 72px;
            height: 36px;
        }

        .home-filter__page-btn {
            flex: 1;
            display: flex;
            background-color: var(--white-color);
            text-decoration: none;
        }

        .home-filter__page-btn--disabled {
            background-color: #f9f9f9;
            cursor: default;
        }

        .home-filter__page-btn--disabled .home-filter__page-icon {
            color: #ccc;
        }

        .home-filter__page-icon:first-child {
            border-right: 1px solid #eee;
        }

        .home-filter__page-icon {
            margin: auto;
            font-size: 1.4rem;
            color: var(--primary-color);
        }


    </style>
    <body>
        <!-- Block Element Modifer-->
        <div class="app">
            <header class="header">
                <div class="grid">
                    <nav class="header_navbar">
                        <ul class="header__navbar-list">
                            <li class="header__navbar-item header__navbar-item--has-qr header__navbar-item--separate">
                                Open Store
                                <!-- Header QR code-->
                                <div class="header__qr">
                                    <img src="./img/qr_code.png" alt="QR code" class="header__qr-img">
                                    <div class="header__qr-apps">
                                        <a href="" class="header__qr-link">
                                            <img src="./img/google_play.png" alt="Google Play" class="header__qr-download-img">
                                        </a>
                                        <a href="" class="header__qr-link">
                                            <img src="./img/app_store.png" alt="App Store" class="header__qr-download-img">
                                        </a> 
                                    </div>
                                </div>
                            </li>
                            <li class="header__navbar-item">
                                <span class="header__navbar-title--no-pointer">Connect</span>
                                <a href="" class="header__navbar-icon-link">
                                    <i class="header__navbar-icon fa-brands fa-facebook"></i>
                                </a>
                                <a href="" class="header__navbar-icon-link">
                                    <i class="header__navbar-icon fa-brands fa-instagram"></i>
                                </a>
                                <a href="" class="header__navbar-icon-link">
                                    <i class="header__navbar-icon fa-brands fa-youtube"></i>
                                </a>
                            </li>
                        </ul>
                        
                        <ul class="header__navbar-list">
                            <li class="header__navbar-item header__navbar-item--has-notify">
                                <a href="" class="header__navbar-item-link">
                                    <i class="header__navbar-icon fas fa-bell"></i>
                                    Notifications
                                </a>
                                <div class="header__notify">
                                    <header class="header__notify-header">
                                        <h3>New Notifications</h3>
                                    </header>
                                    <ul class="header__notify-list">
                                        <li class="header__notify-item header__notify-item--viewed">
                                            <a href="" class="header__notify-link">
                                                <img src="./img/cnc1.png" alt="" class="header__notify-img">
                                                <div class="header__notify-info">
                                                    <span class="header__notify-name">Program 1</span>
                                                    <span class="header__notify-description">Active</span>
                                                </div>
                                            </a>
                                        </li>
                                        <li class="header__notify-item header__notify-item--viewed">
                                            <a href="" class="header__notify-link">
                                                <img src="./img/cnc2.png" alt="" class="header__notify-img">
                                                <div class="header__notify-info">
                                                    <span class="header__notify-name">Program 2</span>
                                                    <span class="header__notify-description">Active</span>
                                                </div>
                                            </a>
                                        </li>
                                        <li class="header__notify-item header__notify-item--viewed">
                                            <a href="" class="header__notify-link">
                                                <img src="./img/cnc3.png" alt="" class="header__notify-img">
                                                <div class="header__notify-info">
                                                    <span class="header__notify-name">Program 3</span>
                                                    <span class="header__notify-description">Shut down </span>
                                                </div>
                                            </a>
                                        </li>
                                    </ul>
                                    <footer class="header__notify-footer">
                                        <a href="" class="header__notify-footer-btn">Show all</a>
                                    </footer>
                                </div>
                            </li>
                            <li class="header__navbar-item">
                                <a href="" class="header__navbar-item-link">
                                    <i class="header__navbar-icon fa-solid fa-circle-info"></i>
                                    Help
                                </a>
                            </li>
                            <!-- <li class="header__navbar-item header__navbar-item--strong header__navbar-item--separate">Sign In</li>
                            <li class="header__navbar-item header__navbar-item--strong">Log In</li> -->
                            <li class="header__navbar-item header__navbar-user">
                                <img src="./img/avatar.png" alt="" class="header__navbar-user-img">
                                <span class="header__navbar-user-name">WIS</span>

                                <ul class="header__navbar-user-menu">
                                    <li class="header__navbar-user-item">
                                        <a href="">My Account</a>
                                    </li>
                                    <li class="header__navbar-user-item">
                                        <a href="">My Address</a>
                                    </li>
                                    <li class="header__navbar-user-item">
                                        <a href="">Activet</a>
                                    </li>
                                    <li class="header__navbar-user-item header__navbar-user-item--separate">
                                        <a href="">Log Out</a>
                                    </li>
                                </ul>
                            </li>
                        </ul>
                    </nav>

                    <!-- Header with search -->
                    <div class="header-with-search">
                        <div class="header__logo">
                            <a href="./index.html" class="header__logo-link">
                                <img src="./img/logo.png" alt="" class="header__logo-img">
                            </a>
                        </div>

                        <div class="header__search">
                            <div class="header__search-input-wrap">
                                <input type="text" class="header__search-input" placeholder="Type to find programing">

                                <!-- Search history-->
                                <div class="header-search-history">
                                    <h3 class="header-search-history-heading">Browsing history</h3>
                                    <ul class="header-search-history-list">
                                        <li class="header-search-history-item">
                                            <a href="">Program 1</a>
                                        </li>
                                        <li class="header-search-history-item">
                                            <a href="">Program 2</a>
                                        </li>
                                    </ul>
                                </div>
                            </div>
                            <div class="header__search-select">
                                <span class="header__search-select-lable">In Program</span>
                                <i class="header__search-select-icon fa-solid fa-chevron-down"></i>

                                <ul class="header__search-option">
                                    <li class="header__search-option-item header__search-option-item--active">
                                        <span>In program</span>
                                        <i class="fas fa-check"></i>
                                    </li>
                                    <li class="header__search-option-item">
                                        <span>Out program</span>
                                        <i class="fas fa-check"></i>
                                    </li>
                                </ul>
                            </div>
                            <button class="header__search-btn">
                                <i class="header__search-btn-icon fa-solid fa-magnifying-glass"></i>
                            </button>
                        </div>

                        <!-- Menu layout -->
                        <div class="header__menu">
                            <div class="header__menu-wrap">
                                <i class="header__menu-icon fa-solid fa-bars"></i>
                                <span class="header__menu-notice">3</span>

                                <!-- No program: header__menu-list--no-menu-->
                                <div class="header__menu-list">
                                    <img src="./img/menu.png" alt="" class="header__menu-no-menu-img">
                                    <span class="header__menu-list-no-menu-msg">
                                        Program not available yet
                                    </span>

                                    <h4 class="header__menu-heading">Program has been added</h4>
                                    <ul class="header__menu-list-iteam">
                                        <!-- Menu item -->
                                        <li class="header__menu-iteam">
                                            <img src="./img/cnc1.png" alt="" class="header__menu-img">
                                            <div class="header__menu-iteam-info">
                                                <div class="header__menu-iteam-head">
                                                    <h5 class="header__menu-iteam-name">Program 1</h5>
                                                    <div class="header__menu-iteam-state-wrap">
                                                        <span class="header__menu-iteam-state">Active</span>
                                                        <i class="header__menu-iteam-icon-state fa-solid fa-bolt"></i>
                                                    </div>
                                                </div>
                                                <div class="header__menu-iteam-body">
                                                    <span class="header__menu-iteam-description">
                                                        Control CNC 3 axis: X Y Z
                                                    </span>
                                                    <span class="header__menu-iteam-stop">Stop</span>
                                                </div>
                                            </div>
                                        </li>
                                        <li class="header__menu-iteam">
                                            <img src="./img/cnc2.png" alt="" class="header__menu-img">
                                            <div class="header__menu-iteam-info">
                                                <div class="header__menu-iteam-head">
                                                    <h5 class="header__menu-iteam-name">Program 2</h5>
                                                    <div class="header__menu-iteam-state-wrap">
                                                        <span class="header__menu-iteam-state">Active</span>
                                                        <i class="header__menu-iteam-icon-state fa-solid fa-bolt"></i>
                                                    </div>
                                                </div>
                                                <div class="header__menu-iteam-body">
                                                    <span class="header__menu-iteam-description">
                                                        Voltage and Current
                                                    </span>
                                                    <span class="header__menu-iteam-stop">Stop</span>
                                                </div>
                                            </div>
                                        </li>
                                        <li class="header__menu-iteam">
                                            <img src="./img/cnc3.png" alt="" class="header__menu-img">
                                            <div class="header__menu-iteam-info">
                                                <div class="header__menu-iteam-head">
                                                    <h5 class="header__menu-iteam-name">Program 3</h5>
                                                    <div class="header__menu-iteam-state-wrap">
                                                        <span class="header__menu-iteam-state">Active</span>
                                                        <i class="header__menu-iteam-icon-state fa-solid fa-bolt"></i>
                                                    </div>
                                                </div>
                                                <div class="header__menu-iteam-body">
                                                    <span class="header__menu-iteam-description">
                                                        Accelstepper/Speedstepper
                                                    </span>
                                                    <span class="header__menu-iteam-stop">Stop</span>
                                                </div>
                                            </div>
                                        </li>
                                    </ul>
                                    <a href="#" class="header__menu-view-menu btn btn--primary">Check program</a>
                                </div>
                            </div>
                        </div>
                    </div>
                </div>
            </header>

            <div class="app__container">
                <div class="grid">
                    <div class="grid__row app__content">
                        <div class="grid__column-2">
                            <nav class="category">
                                <h3 class="category__heading">
                                    <i class="category__heading-icon fas fa-list"></i>
                                    Category
                                </h3>
            
                                <ul class="category-list">
                                    <li class="category-item category-item--active">
                                        <a href="" class="category-item__link">Program 1</a>
                                    </li>
                                    <li class="category-item">
                                        <a href="" class="category-item__link">Program 2</a>
                                    </li>
                                    <li class="category-item">
                                        <a href="" class="category-item__link">Program 3</a>
                                    </li>
                                </ul>
                            </nav>
                        </div>
    
                        <div class="grid__column-10">
                            <div class="home-filter">
                                <span class="home-filter__label">Operating Panel</span>
                                <button class="home-filter__btn btn btn--primary">Start</button>
                                <button class="home-filter__btn btn">Stop</button>
                                <button class="home-filter__btn btn">Reset</button>

                                <div class="select-input">
                                    <span class="select-input__label">Speed</span>
                                    <i class="select-input__icon fa-solid fa-chevron-down"></i>

                                    <!-- List option-->
                                    <ul class="select-input__list">
                                        <li class="select-input__item">
                                            <a href="" class="select-input__link">Low</a>
                                        </li>
                                        <li class="select-input__item">
                                            <a href="" class="select-input__link">Normal</a>    
                                        </li>
                                        <li class="select-input__item">
                                            <a href="" class="select-input__link">Fast</a>
                                        </li>
                                    </ul>
                                </div>

                                <div class="home-filter__page">
                                    <span class="home-filter__page-num">
                                        <span class="home-fileter__page-current">1</span>/3
                                    </span>
                                    <div class="home-filter__page-control">
                                        <a href="" class="home-filter__page-btn home-filter__page-btn--disabled">
                                            <i class="home-filter__page-icon fa-solid fa-chevron-left"></i>
                                        </a>
                                        <a href="" class="home-filter__page-btn">
                                            <i class="home-filter__page-icon fa-solid fa-chevron-right"></i>
                                        </a>
                                    </div>
                                </div>
                            </div>

                            <div class="home-product">
                                <!-- Grid -> Row -> Colum -->
                                <div class="grid__row">
                                    <div class="grid__column-5">
                                        <!-- Product item -->
                                        <div class="home-product-item">
                                            <div class="home-product-item__img" style="background-image: url(https://www.bing.com/images/search?view=detailV2&ccid=0lz%2fYP2h&id=AA21A71EC291C7352887E146F555D5EE5A52F34E&thid=OIP.0lz_YP2hB6ubWT1pgr7_kQHaEG&mediaurl=https%3a%2f%2fbizweb.dktcdn.net%2f100%2f337%2f129%2ffiles%2fmay-cat-plasma-bc04837e-b993-4e6c-8f9a-c0f723f1916b.jpg%3fv%3d1581563861255&cdnurl=https%3a%2f%2fth.bing.com%2fth%2fid%2fR.d25cff60fda107ab9b593d6982beff91%3frik%3dTvNSWu7VVfVG4Q%26pid%3dImgRaw%26r%3d0&exph=974&expw=1762&q=may+cnc&simid=608046591034332680&FORM=IRPRST&ck=7E72DF358509F0FD71F7099353607C73&selectedIndex=10&itb=0);">
                                                <h4 class="home-product-item__name">Volte</h4>
                                                <div class="home-product-item__price">
                                                    <span class="home-product-item__price-old">120000đ</span>
                                                    <span class="home-product-item__price-current">99000đ</span>
                                                </div>
                                                <div class="home-product-item__action">
                                                    <span class="home-product-item__like">
                                                        <i class="far fa-heart"></i>
                                                        <!-- <i class="fas fa-heart"></i> -->
                                                    </span>
                                                    <div class="home-product-item__rating">
                                                        <i class="fas fa-star"></i>
                                                        <i class="fas fa-star"></i>
                                                        <i class="fas fa-star"></i>
                                                        <i class="fas fa-star"></i>
                                                        <i class="fas fa-star"></i>
                                                    </div>
                                                </div>
                                                <div class="home-product-item__origin">
                                                    <span class="home-product-item__brand">Whoo</span>
                                                    <span class="home-product-item__origin-name">Japan</span>
                                                </div>
                                                <div class="home-product-item__favourite">
                                                    <i class="fas fa-check"></i>
                                                    Yeu Thich
                                                </div>
                                                <div class="home-product-item__sale-off">
                                                    <span class="home-product-item__sale-off-percent">10%</span>
                                                    <span class="home-product-item__sale-off-label">Giam</span>
                                                </div>
                                            </div>
                                        </div>
                                    </div>
                                </div>
                            </div>
                        </div>
                    </div>
                </div>
            </div>

            <footer class="footer">

            </footer>
        </div>

        <!--  Modal layout -->
        <!-- <div class="modal">
            <div class="modal__overlay"></div>

            <div class="modal__body"> -->
                    
                <!-- Sign Up form -->
                <!-- <div class="auth-form">

                    <div class="auth-form__container">
                        <div class="auth-form__header">
                            <h3 class="auth-form__heading">Sign Up</h3>
                            <span class="auth-form__switch-btn">Log In</span>
                        </div>
                        
                        <div class="auth-form__form">
                            <div class="auth-form__group">
                                <input type="text" class="auth-form__input" placeholder="Your Email">
                            </div>
                            <div class="auth-form__group">
                                <input type="password" class="auth-form__input" placeholder="Your Passwword">
                            </div>
                            <div class="auth-form__group">
                                <input type="password" class="auth-form__input" placeholder="Confirm Your Passwword ">
                            </div>
                        </div>
    
                        <div class="auth-form__aside">
                            <p class="auth-form__policy-text">
                                By registering, you're confirming your agreement with TRITECH CNC's terms
                                <a href="" class="auth-form__text-link">Terms of Service</a> &
                                <a href="" class="auth-form__text-link">Privacy Policy</a>
                            </p>
                        </div>
    
                        <div class="auth-form__controls">
                            <button class="btn btn--normal auth-form__controls-return">RETURN</button>
                            <button class="btn btn--primary">SIGN UP</button>
                        </div>
                    </div> 

                    <div class="auth-form__socials">
                        <a href="" class="auth-form__socials--facebook btn btn--size-s btn--with-icon">
                            <i class="auth-form__socials-icon fab fa-facebook-square"></i>
                            <span class="auth-form__socials-title">
                                Connect with Facebook
                            </span>
                        </a> 
                        <a href="" class="auth-form__socials--google btn btn--size-s btn--with-icon">
                            <i class="auth-form__socials-icon fab fa-google"></i>
                            <span class="auth-form__socials-title">
                                Connect with Google
                            </span>
                        </a> 
                    </div>

                </div> -->

                <!-- Log In form -->
                <!-- <div class="auth-form">

                    <div class="auth-form__container">
                        <div class="auth-form__header">
                            <h3 class="auth-form__heading">Log In</h3>
                            <span class="auth-form__switch-btn">Sign Up</span>
                        </div>
                        
                        <div class="auth-form__form">
                            <div class="auth-form__group">
                                <input type="text" class="auth-form__input" placeholder="Your Email">
                            </div>
                            <div class="auth-form__group">
                                <input type="password" class="auth-form__input" placeholder="Your Passwword">
                            </div>
                        </div>
    
                        <div class="auth-form__aside">
                            <div class="auth--form__help">
                                <a href="" class="auth-form__help-link auth-form__help-forgot">Forgot Password</a>
                                <span class="auth-form__help-separate"></span>
                                <a href="" class="auth-form__help-link ">Need Help ?</a>
                            </div>
                        </div>
    
                        <div class="auth-form__controls">
                            <button class="btn btn--normal auth-form__controls-return">RETURN</button>
                            <button class="btn btn--primary">Log In</button>
                        </div>
                    </div> 

                    <div class="auth-form__socials">
                        <a href="" class="auth-form__socials--facebook btn btn--size-s btn--with-icon">
                            <i class="auth-form__socials-icon fab fa-facebook-square"></i>
                            <span class="auth-form__socials-title">
                                Login with Facebook
                            </span>
                        </a> 
                        <a href="" class="auth-form__socials--google btn btn--size-s btn--with-icon">
                            <img src="./img/google.png" alt="" class="auth-form__socials-icon auth-form__socials-icon-google">
                            <span class="auth-form__socials-title auth-form__socials-title-google">
                                Login with Google
                            </span>
                        </a> 
                    </div>

                </div>
            </div>
        </div> -->
    </body>
</html>
)=====";